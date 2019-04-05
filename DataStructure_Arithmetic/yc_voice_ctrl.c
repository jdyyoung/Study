#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

#include "ak_common.h"
#include "ak_global.h"
#include "ak_thread.h"
#include "ak_config.h"
#include "ak_ao.h"
#include "ak_adec.h"
#include "ak_demux.h"
#include "list.h"
#include "yc_voice_ctrl.h"
#include "yc_story.h"
#include "yc_interaction.h"

#define RECORD_READ_LEN 4096 /* read audio file buffer length */
#define PLAY_COUNT 10
#define FRAME_BUFFER 4096
#define FILE_NAME_LEN (200) //max file name length

struct audio_node
{
	char file_name[FILE_NAME_LEN]; //include absolutely path
	unsigned long start_ms;
	unsigned long end_ms;
	struct list_head list; //hang to file_list
	FUNC n_ai_cb;
};

/* voice play control struct */
struct play_ctrls
{
	int run_flag;	//run flag
	int stop_flag;         //run flag
	int pause_flag;        //pause_flag
	int play_complit_flag; //play complit flag
	unsigned long pause_ms;
	unsigned long totaltime_ms;
	void *ao_handle;    //ao handle
	void *adec_handle;  //adec handle
	void *demux_handle; //adec stream handle
	ak_mutex_t play_mutex;
	ak_sem_t play_sem;	//play sem
	pthread_t play_tid;         //play thread id
	struct list_head file_list; //audio file queue
	FUNC ai_cb;	       //AI play audio callback
};

struct play_ctrls play_ctrl = {0};
int playing_flag = 0;
int play_story_end_flag = 0;
char last_play_file[256] = {0};

int voice_add_play(const char *file_name);
static int get_play_pcm_interval(struct audio_param *decode,
			   int len)
{
	int sample_rate = decode->sample_rate > 0 ? decode->sample_rate : 8000;
	int sample_bit = decode->sample_bits == 16 ? decode->sample_bits : 16;

	int interval = (len * 8000 / sample_rate / sample_bit / decode->channel_num) >> 1;

	//ak_print_error_ex(" sample_rate=%d, sample_bytes=%d, interval=%d, channel_num=%d,len=%d\n",
	//			 sample_rate, sample_bit, interval, decode->channel_num, len);

	return interval;
}

static void wait_play_finished(void *ao_handle)
{
	enum ao_play_status pre_status = AO_PLAY_STATUS_RESERVED;
	enum ao_play_status cur_status = AO_PLAY_STATUS_RESERVED;
	int total_time = 0;

	do
	{
		/* get ao status */
		cur_status = ak_ao_get_play_status(ao_handle);
		if (pre_status != cur_status)
		{
			pre_status = cur_status;
			switch (cur_status)
			{
			case AO_PLAY_STATUS_READY:
				ak_print_normal("play status=%d, ready to play\n", cur_status);
				break;
			case AO_PLAY_STATUS_PLAYING:
				ak_print_normal("play status=%d, playing, wait time: %d\n",
					      cur_status, total_time);
				break;
			case AO_PLAY_STATUS_FINISHED:
				ak_print_normal("play status=%d, play finished\n", cur_status);
				break;
			default:
				cur_status = AO_PLAY_STATUS_FINISHED;
				ak_print_normal("play status=%d, reserved\n", cur_status);
				break;
			}
		}

		/* wait the data play finish */
		if (AO_PLAY_STATUS_FINISHED != cur_status)
		{
			ak_sleep_ms(10);
			total_time += 10;
		}
		// ak_print_error_ex("cur_status =%d\n", cur_status);
	} while (AO_PLAY_STATUS_FINISHED != cur_status);
}

/**
 * decode_and_play - send the stream data to decode
 * @adec_handle[IN]: adec handle
 * @data[IN]: audio file data
 * @read_len[IN]: audio file data length
 * notes:
 */
static void send_decode_stream(void *adec_handle, unsigned char *data, int read_len)
{
	int send_len = 0;
	int dec_len = 0;

	while (read_len > 0)
	{
		/* send stream to decode */
		dec_len = ak_adec_send_stream(adec_handle, &data[send_len], read_len, 0);
		if (dec_len < 0)
		{
			ak_print_error_ex("write pcm to DA error!\n");
			break;
		}

		read_len -= dec_len;
		send_len += dec_len;
	}
}

/**
 * get_pcm_frame_and_play - get pcm frame and use ao play
 * @adec_handle[IN]: adec handle
 * @ao_handle[IN]: ao handle
 * @play_len[IN]: play data len 
 * return: decode frame len
 * notes:
 */
//FILE *pcm_fp = NULL;
int no_frame_count = 0;
static int get_pcm_frame_and_play(void *adec_handle, void *ao_handle)
{
	int out_size = 0;
	int cur_size = 0;
	unsigned char frame_buf[FRAME_BUFFER];
	no_frame_count = 0;
	int total_len = 0;
	// pr("循环2----out_size:%d\n", out_size);
	do
	{

		out_size = ak_adec_get_frame(adec_handle, &frame_buf[cur_size]);
		if (out_size > 0)
		{
			//fwrite(frame_buf, out_size, 1, pcm_fp);
			no_frame_count = 0;
			cur_size += out_size;
			if (cur_size > 0)
			{
				if (AK_FAILED == ak_ao_send_frame(ao_handle,
							    frame_buf, cur_size, -1))
				{
					ak_print_error_ex("write pcm to DA error\n");
				}
				cur_size = 0;
				total_len += out_size;
			}
		}
		else
		{
			++no_frame_count;
		}

	} while (out_size > 0 && voice_is_playing());
	// pr("循环2----o退出:%d\n", out_size);
	return total_len;
}

/**
 * print_playing_dot - print . when playing every second
 * notes:
 */
static void print_playing_dot(void)
{
	static unsigned char first_flag = AK_TRUE;
	static struct ak_timeval cur_time;
	static struct ak_timeval print_time;

	if (first_flag)
	{
		first_flag = AK_FALSE;
		ak_get_ostime(&cur_time);
		ak_get_ostime(&print_time);
		ak_print_normal("\n.");
	}

	/* get time */
	ak_get_ostime(&cur_time);
	if (ak_diff_ms_time(&cur_time, &print_time) >= 1000)
	{
		ak_get_ostime(&print_time);
		ak_print_normal(".");
	}
}

//int count = 0;
//FILE *fp = NULL;
int no_stream_count = 0;
static int read_record_file_and_play(
    unsigned long start_ts, unsigned long end_ts,
    struct audio_param *info)
{
	struct demux_stream *stream = NULL;
	int ret = ak_demux_seek_to_keyframe(play_ctrl.demux_handle, start_ts, 0);
	int frame_len = 0;
	int play_interval = 0;
	int play_ts = 0;

	while (!play_ctrl.stop_flag)
	{

		/* read the record file stream */
		stream = ak_demux_get_audio(play_ctrl.demux_handle);
		if (!stream)
		{
			ak_adec_notice_stream_end(play_ctrl.adec_handle);
			no_stream_count++;
		}

		if (stream && stream->len > 0)
		{
			//			fwrite(stream->data, stream->len, 1, fp);
			//ak_print_notice("stream length:%d----\n", stream->len);
			/* send stream to decode */
			send_decode_stream(play_ctrl.adec_handle, stream->data, stream->len);
			no_stream_count = 0;
		}

		frame_len = get_pcm_frame_and_play(play_ctrl.adec_handle, play_ctrl.ao_handle);

		if (no_frame_count > 3 || no_stream_count > 6)
		{
			play_ctrl.play_complit_flag = 1;
			break;
		}
			

		/* calculate the interval */
		if (frame_len > 0)
		{
			play_interval = get_play_pcm_interval(info, frame_len);
			//ak_print_normal("decode length:%d----play_interval:%d\n", frame_len, play_interval);
			play_ts += play_interval;
			// print_playing_dot();
		}

		if (end_ts != start_ts && play_ts >= (end_ts - start_ts))
		{
			ak_print_error("play_ts=%d, %ld\n", play_ts, (end_ts - start_ts));
			play_ctrl.play_complit_flag = 1;
			break;
		}
		// if (!stream)
		// {
		// 	play_ctrl.play_complit_flag = 1;
		// 	ak_print_normal_ex("================ak_adec_notice_stream_end = NULL,exit =======================\n");
		// 	break;
		// }
		if(NULL!= stream){
			ak_demux_free_data(stream);
			stream = NULL;
		}

		ak_sleep_ms(10);
	}
	if (play_ctrl.pause_flag == AK_TRUE)
	{
		play_ctrl.pause_ms = play_ts;
	}
	wait_play_finished(play_ctrl.ao_handle);
	if(NULL!= stream){
		ak_demux_free_data(stream);
		stream = NULL;
	}
	/* read over the file, must call ak_adec_read_end */
	ak_print_notice_ex("%s exit\n", __func__);
	// pr("循环1----frame_len:%d\n", frame_len);
	return ret;
}

static FILE *open_record_file(const char *record_file)
{
	FILE *fp = fopen(record_file, "r");
	if (NULL == fp)
	{
		ak_print_error_ex("open %s failed\n", record_file);
		return NULL;
	}

	ak_print_normal("open record file: %s OK\n", record_file);
	return fp;
}
static void read_record_file(void *adec_handle, FILE *fp, int dec_type)
{
	int read_len = 0;
	int total_len = 0;
	unsigned char data[RECORD_READ_LEN] = {0};

	/* bind audio output & decode, request stream */
	void *stream_handle = ak_adec_request_stream(play_ctrl.ao_handle, adec_handle);
	if (NULL == stream_handle)
	{
		ak_print_normal("\t ak_adec_request_stream failed! \n");
		return;
	}

	/* amr file,have to skip the head 6 byte */
	if (AK_AUDIO_TYPE_AMR == dec_type)
	{
		read_len = fread(data, 1, 6, fp);
	}

	while (!play_ctrl.stop_flag)
	{
		/* read the record file stream */
		memset(data, 0x00, sizeof(data));
		read_len = fread(data, sizeof(char), sizeof(data), fp);

		if (read_len > 0)
		{
			total_len += read_len;
			/* play roop */
			send_decode_stream(adec_handle, data, read_len);
			print_playing_dot();
			ak_sleep_ms(10);
		}
		else if (0 == read_len)
		{
			/* read to the end of file */
			ak_print_normal("\n\tread to the end of file\n");
			break;
		}
		else
		{
			ak_print_error("\nread, %s\n", strerror(errno));
			break;
		}
	}
	/* read over the file, must call ak_adec_read_end */
	ak_adec_notice_stream_end(adec_handle);
	/* decode end, cancel stream */
	ak_adec_cancel_stream(stream_handle);
	stream_handle = NULL;
	ak_print_notice_ex("%s exit\n", __func__);
}

void voice_set_volume(int volume)
{

	if (play_ctrl.ao_handle != NULL)
	{
		ak_ao_set_volume(play_ctrl.ao_handle, volume);
	}
	else
	{
		ak_print_normal("ao已关闭，无法设置声音\n");
	}
}

int voice_is_playing()
{
	return !play_ctrl.stop_flag;
}

void voice_up()
{
	struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
	globle_config->volume++;
	ak_print_normal("声音增加，音量:%d\n", globle_config->volume);
	if (globle_config->volume > 12)
	{
		globle_config->volume = 12;
	}
	ak_config_set_system_user(globle_config);
	voice_set_volume(globle_config->volume);
	if (!voice_is_playing())
	{
		yc_interaction_func(STATUS_VOICE_CHANGE);
	}
}
void voice_down()
{
	struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
	globle_config->volume--;
	ak_print_normal("声音降低，音量:%d\n", (globle_config->volume));
	if (globle_config->volume < 0)
	{
		globle_config->volume = 0;
	}
	ak_config_set_system_user(globle_config);
	voice_set_volume(globle_config->volume);
	if (!voice_is_playing())
	{
		yc_interaction_func(STATUS_VOICE_CHANGE);
	}
}

void close_ao_handler()
{
	if (NULL != play_ctrl.ao_handle)
	{
		// voice_set_volume(1);
		/* ao disable speaker */
		pr("关闭ao---\n");
		ak_ao_enable_speaker(play_ctrl.ao_handle, AUDIO_FUNC_DISABLE);
		/* ao close */
		ak_ao_close(play_ctrl.ao_handle);
		play_ctrl.ao_handle = NULL;
	}
	if (NULL != play_ctrl.demux_handle)
	{
		pr("关闭ak_demux_close---\n");
		ak_demux_close(play_ctrl.demux_handle);
		play_ctrl.demux_handle = NULL;
	}
	playing_flag = 0;
}

void close_adec_handle()
{
	/* adec close */
	if (NULL != play_ctrl.adec_handle)
	{
		pr("关闭ak_adec_close---\n");
		ak_adec_close(play_ctrl.adec_handle);
		play_ctrl.adec_handle = NULL;
	}
}


void voice_stop_play()
{

	if (play_ctrl.stop_flag)
	{
		pr("音频已关闭\n");
		return;
	}

	play_ctrl.pause_flag = 0;
	play_story_end_flag = 0;
	play_ctrl.stop_flag = 1;
	struct audio_node *entry = NULL;
	struct audio_node *ptr = NULL;

	//如果是停止标志，说明是主动停止
	printf("%s开始清空链表\n", __func__);

	if(list_empty(&play_ctrl.file_list)){
		pr("list is null ----------------------------------------------\n");
	}
	else{
		pr("list not null ----------------------------------------------\n");
	}
	ak_thread_mutex_lock(&play_ctrl.play_mutex);
	list_for_each_entry_safe(entry, ptr, &(play_ctrl.file_list), list)
	{
		list_del_init(&(entry->list));
		pr("删除音频:%s，free_addr=%p\n", entry->file_name,entry);
		ak_free(entry);
		entry = NULL;
	}
	ak_thread_mutex_unlock(&play_ctrl.play_mutex);
	if(list_empty(&play_ctrl.file_list)){
		pr("list is null ----------------------------------------------\n");
	}
	printf("%s结束清空链表\n", __func__);
	//休眠50ms等待线程退出结束
	//usleep(50 * 1000);
	printf("停止音频\n");
}


int voice_add_play_range(const char *file_name, unsigned long start_ms, unsigned long end_ms)
{
	// if(file_name)
	if (!play_ctrl.run_flag)
	{
		ak_print_warning_ex("uninitial\n");
		return AK_FAILED;
	}
	ak_print_warning_ex("adding file: %s\n", file_name);
	// struct audio_node *f = (struct audio_node *)ak_calloc(1, sizeof(struct audio_node));
	struct audio_node *f = (struct audio_node *)malloc(sizeof(struct audio_node));
	if (!f)
	{
		ak_print_error_ex("no memory\n");
		return AK_FAILED;
	}

	// if (strlen(file_name) > FILE_NAME_LEN)
	// 	ak_print_warning_ex("%s's len big than %d\n", file_name, FILE_NAME_LEN);

	strncpy(f->file_name, file_name, FILE_NAME_LEN);
	f->n_ai_cb = play_ctrl.ai_cb;
	play_ctrl.ai_cb = NULL;
	pr("----add---startTimt:%lu,endTime：%lu\n", start_ms, end_ms);
	f->start_ms = start_ms;
	f->end_ms = end_ms;

	/* get file info: sample_rate, file encode type and so on */

	ak_thread_mutex_lock(&play_ctrl.play_mutex);
	list_add_tail(&f->list, &play_ctrl.file_list);
	ak_thread_mutex_unlock(&play_ctrl.play_mutex);
	ak_thread_sem_post(&play_ctrl.play_sem);

	return AK_SUCCESS;
}

int voice_add_play(const char *file_name)
{
	if (play_ctrl.pause_flag == AK_TRUE)
	{
		play_ctrl.pause_flag = AK_FALSE;
		return voice_add_play_range(file_name, play_ctrl.pause_ms, play_ctrl.totaltime_ms);
	}
	else
	{
		return voice_add_play_range(file_name, 0, 0);
	}
}

int voice_add_play_ai(const char *file_name, FUNC cb)
{
	play_ctrl.ai_cb = cb;
	voice_add_play(file_name);
}
/*node 在 list里面则返回1，否则返回0*/
static int check_node_is_list(struct audio_node *node){
	if(NULL == node){
		return 0;
	}
	//return 0;
	struct audio_node *pos = NULL;
	if(list_empty(&play_ctrl.file_list)){
		//pr("list is null===================================\n");
		return 0;
	}
	
	list_for_each_entry(pos,&play_ctrl.file_list,list){
		//pr("pos = %p==================================\n",pos);
		if(pos == node){
			return 1;
		}
	}
	return 0;
}

static void *play_work_th(void *arg)
{
	char file_name_buf[256];
	long int tid = ak_thread_get_tid();
	struct audio_node *node = NULL;
	ak_print_normal_ex("thread id: %ld\n", tid);
	ak_thread_set_name("voice");

	while (play_ctrl.run_flag)
	{
		ak_thread_sem_wait(&play_ctrl.play_sem);
		play_ctrl.stop_flag = 0;
		ak_thread_mutex_lock(&play_ctrl.play_mutex);
		node = list_first_entry_or_null(&play_ctrl.file_list,
					  struct audio_node, list);
		ak_thread_mutex_unlock(&play_ctrl.play_mutex);
		if (node == NULL || !strlen(node->file_name))
		{
			printf("空节点，释放锁\n");
			if (node != NULL)
			{
				ak_thread_mutex_lock(&play_ctrl.play_mutex);
				if(check_node_is_list(node)){
					list_del_init(&node->list);
					ak_free(node);
					node = NULL;
				}
				ak_thread_mutex_unlock(&play_ctrl.play_mutex);
			}
			continue;
		}

		playing_flag = 1;

		play_ctrl.demux_handle = ak_demux_open((char *)node->file_name, 0);
		if (play_ctrl.demux_handle == NULL)
		{
			ak_print_error_ex("demux open %s failed\n", node->file_name);
			ak_thread_mutex_lock(&play_ctrl.play_mutex);
			if(check_node_is_list(node)){
				list_del_init(&node->list);
				ak_free(node);
				node = NULL;
			}
			ak_thread_mutex_unlock(&play_ctrl.play_mutex);

			continue;
		}

		struct ak_demux_media_info media_info;
		if (ak_demux_get_media_info(play_ctrl.demux_handle, &media_info))
		{
			ak_print_error_ex("demux open %s failed\n", node->file_name);
			ak_thread_mutex_lock(&play_ctrl.play_mutex);
			if(check_node_is_list(node)){
				list_del_init(&node->list);
				ak_free(node);
				node = NULL;
			}
			ak_thread_mutex_lock(&play_ctrl.play_mutex);
			close_ao_handler();
			continue;
		}

		/* ao open */
		struct pcm_param param;
		param.sample_rate = media_info.audio_info.sample_rate;
		param.sample_bits = media_info.audio_info.sample_bits;
		param.channel_num = media_info.audio_info.channel_num;

		play_ctrl.ao_handle = ak_ao_open(&param);
		if (NULL == play_ctrl.ao_handle)
		{
			ak_print_normal("\t ak_ao_open failed !\n");
			close_ao_handler();
			continue;
		}

		/* enable speaker must set before ak_ao_send_frame */
		ak_ao_enable_speaker(play_ctrl.ao_handle, AUDIO_FUNC_ENABLE);

		/* volume is from 0 to 12,volume 0 is mute */
		// ak_ao_set_volume(play_ctrl.ao_handle, 4);
		struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
		voice_set_volume(globle_config->volume);
		/* ak_ao_set_resample have to use before ak_ao_send_frame */
		ak_ao_set_resample(play_ctrl.ao_handle, AUDIO_FUNC_ENABLE);

		/* before ak_ao_send_frame,must clear frame buffer */
		ak_ao_clear_frame_buffer(play_ctrl.ao_handle);

		/***************************
			step 3: open adec
		***************************/
		struct audio_param adec_param = {0};
		adec_param.type = media_info.audio_info.type;

		/* sample bits only support 16 bit */
		adec_param.sample_bits = 16;

		/* driver set to AUDIO_CHANNEL_MONO */
		adec_param.channel_num = media_info.audio_info.channel_num;
		adec_param.sample_rate = media_info.audio_info.sample_rate;

		/* open adec */
		play_ctrl.adec_handle = ak_adec_open(&adec_param);

		pr("打开demux 成功，将要打开ao \n");

		if (NULL == play_ctrl.adec_handle)
		{
			ak_print_normal("\t ak_adec_open failed! \n");
			close_adec_handle();
			continue;
		}
		pr("媒体时长ms=%d\n", media_info.totaltime_ms);
		play_ctrl.stop_flag = 0;
		if (strncmp(node->file_name, "/usr/share", strlen("/usr/share")))
		{
			yc_interaction_func(STATUS_VOICE_PLAY_START);
		}
		pr("进入播放循环\n");
		if (node->start_ms != node->end_ms)
		{
			read_record_file_and_play(node->start_ms, node->end_ms, &adec_param);
		}
		else
		{
			play_ctrl.totaltime_ms = media_info.totaltime_ms;
			read_record_file_and_play(0, media_info.totaltime_ms, &adec_param);
		}

		close_adec_handle();
		close_ao_handler();
		pr("退出播放循环\n");
		if (!play_ctrl.stop_flag)
		{
			if (node != NULL)
			{
				ak_print_normal("----- %s exit -----\n", node->file_name);
				if (strncmp(node->file_name, "/usr/share", strlen("/usr/share")))
				{
					yc_interaction_func(STATUS_VOICE_PLAY_STOP);
				}
				if (node->n_ai_cb != NULL)
				{
					node->n_ai_cb();
					node->n_ai_cb = NULL;
					if (play_ctrl.pause_flag == AK_FALSE)
					{
						//绘本识图和本地文件，播放完了不删
						remove(node->file_name);
						sync();
					}
				}
				printf("结束播放音频\n");
				strcpy(file_name_buf, node->file_name);
				ak_thread_mutex_lock(&play_ctrl.play_mutex);
				if(check_node_is_list(node)){
					list_del_init(&node->list);
					pr("node addr is %p==========================================\n",node);
					ak_free(node);
					node = NULL;
				}
				ak_thread_mutex_unlock(&play_ctrl.play_mutex);
			}

			if (1 == play_ctrl.play_complit_flag)
			{
				play_ctrl.play_complit_flag = 0;
				if (!strncmp(file_name_buf, "/usr/stories", strlen("/usr/stories")) || !strncmp(file_name_buf, "/mnt/music", strlen("/mnt/music")))
				{
					printf("循环播放下一首\n");
					next_story();
				}
			}

			if (play_story_end_flag)
			{
				yc_interaction_func(STATUS_PLAY_STORY_END);
			}
			play_story_end_flag = 0;
		}
		play_ctrl.stop_flag = 1;
		//ak_sleep_ms(50);
	}
	ak_print_normal_ex("exit ..., tid: %ld\n", tid);

	return NULL;
}
void voice_pause()
{
	//停止现在播放的音频
	play_ctrl.stop_flag = 1;
	play_ctrl.pause_flag = 1;
	//记录停止时的时间搓
}

int init_voice_ctrl()
{
	printf("初始化音频管理\n");
	memset(&play_ctrl, 0, sizeof(play_ctrl));
	INIT_LIST_HEAD(&play_ctrl.file_list);
	play_ctrl.ai_cb = NULL;
	ak_thread_mutex_init(&play_ctrl.play_mutex);
	ak_thread_sem_init(&play_ctrl.play_sem, 0);
	play_ctrl.run_flag = AK_TRUE;
	pthread_create(&play_ctrl.play_tid, NULL, play_work_th, NULL);

	return AK_SUCCESS;
}
int exit_voice_ctrl(void)
{
	printf("退出音频管理\n");
	if (!play_ctrl.run_flag)
	{
		ak_print_warning_ex("uninit\n");
		return -1;
	}

	voice_stop_play();

	play_ctrl.run_flag = 0;
	close_adec_handle();
	close_ao_handler();
	ak_thread_mutex_destroy(&play_ctrl.play_mutex);
	ak_thread_sem_post(&play_ctrl.play_sem);
	if (play_ctrl.play_tid != 0)
	{
		ak_thread_join(play_ctrl.play_tid);
	}

	ak_thread_sem_destroy(&play_ctrl.play_sem);

	ak_print_normal_ex("exit ...\n");

	return 0;
}

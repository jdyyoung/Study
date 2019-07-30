#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
#include "yc_flash_save_volume.h"

#define FLASH_SAVE_VOLUME
#ifdef FLASH_SAVE_VOLUME
#define PARTITION_MAC 2
#define VOICE_MAX_VOLUME 6
#define VOICE_MIN_VOLUME 1

#endif

#define FRAME_BUFFER 1024
#define FILE_NAME_LEN (200) //max file name length

#define CMD_SERVERD_TCP_PORT (80)
//#define CMD_SERVERD_DOMAIN	"protected-hd-01.0mzl.com"
// #define CMD_SERVERD_DOMAIN "echocloud-voice.oss-cn-beijing.aliyuncs.com"

#define BUF_LEN 1024
//#define HTTP_MSG	"GET /cia/musics/Jolv21xEOLtHns7Lyma4syFkQyXWPfMIufQg0vySFeMA4v1nrH91bKf0XuJ7emvR.mp3 HTTP/1.1\r\nAccept: text/html, application/xhtml+xml, image/jxr,*/* \r\nAccept-Language: zh-CN\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko\r\nAccept-Encoding: gzip, deflate\r\nHost: protected-hd-01.0mzl.com\r\nConnection: Keep-Alive\r\n\r\n"
// #define HTTP_MSG "GET /optimus/tts/Ia4pJpK46iP4UlSD7NW4VEDqEclDf5PyGGMayEwiD3DbgouSgSKgcjhhd7ykrkJe.mp3 HTTP/1.1\r\nAccept: text/html, application/xhtml+xml, image/jxr, */* \r\nAccept-Language: zh-CN\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko\r\nAccept-Encoding: gzip, deflate\r\nHost: echocloud-voice.oss-cn-beijing.aliyuncs.com\r\nConnection: Keep-Alive\r\n\r\n"
#define HTTP_MSG "GET %s HTTP/1.1\r\nAccept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8 \r\nAccept-Language:zh-CN,zh;q=0.9,en;q=0.8\r\nCache-Control:max-age=0\r\nUser-Agent:Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36\r\nAccept-Encoding:gzip, deflate\r\nHost: %s\r\nConnection:Keep-Alive\r\n\r\n"

struct audio_node
{
	char file_name[FILE_NAME_LEN]; //include absolutely path
	unsigned long start_ms;
	unsigned long end_ms;
	struct list_head list; //hang to file_list
	FUNC n_ai_cb;
	int is_pause_play;
};

/* voice play control struct */
struct play_ctrls
{
	int run_flag;	//run flag
	int stop_flag;         //run flag
	int pause_flag;        //pause_flag
	int play_complit_flag; //play complit flag
	unsigned long pause_ms;
	unsigned long pause_totaltime_ms;
	char pause_file[FILE_NAME_LEN];
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
int play_story_end_flag = 0;
char last_play_file[256] = {0};
int sleep_warning_flag = 0;
static int decode_close_flag = 1;
int voice_add_play(const char *file_name);
static int yc_check_mp3_is_warning(const char *file_name);
static int get_play_pcm_interval(struct ak_demux_media_info *decode,
			   int len)
{
	int sample_rate = decode->audio_info.sample_rate > 0 ? decode->audio_info.sample_rate : 8000;
	int sample_bit = decode->audio_info.sample_bits == 16 ? decode->audio_info.sample_bits : 16;

	int interval = (len * 8000 / sample_rate / sample_bit / decode->audio_info.channel_num) >> 1;

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
	} while (AO_PLAY_STATUS_FINISHED != cur_status && !play_ctrl.stop_flag);
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
	// yc_print_time("循环2----o退出:%d\n", out_size);
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
enum _EXIT_PLAY
{
	STOP_EXIT = 0,
	PAUSE_EXIT = 1,
	NORNAL_EXIT = 2,
};
static int read_record_file_and_play(
    unsigned long start_ts, unsigned long end_ts,
    struct ak_demux_media_info *info)
{
	struct demux_stream *stream = NULL;
	int ret = STOP_EXIT;
	int frame_len = 0;
	int play_interval = 0;
	int play_ts = 0;

	ak_demux_seek_to_keyframe(play_ctrl.demux_handle, start_ts, 0);
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
			ret = NORNAL_EXIT;
			break;
		}

		/* calculate the interval */
		if (frame_len > 0)
		{
			play_interval = get_play_pcm_interval(info, frame_len);
			play_ts += play_interval;
			//pr("decode length:%d----play_interval:%d play_ts:%d play_ts_now:%d\n", frame_len, play_interval, play_ts , play_ts + start_ts);
			// print_playing_dot();
		}
		if (play_ctrl.pause_flag)
		{
			play_ctrl.pause_ms = play_ts + start_ts;
			pr("pause_ms = %lu\n", play_ctrl.pause_ms);
			ret = PAUSE_EXIT;
			break;
		}

		if (end_ts != start_ts && play_ts >= (end_ts - start_ts))
		{
			ak_print_error("play_ts=%d, %ld\n", play_ts, (end_ts - start_ts));
			play_ctrl.play_complit_flag = 1;
			ret = NORNAL_EXIT;
			break;
		}
		// if (!stream)
		// {
		// 	play_ctrl.play_complit_flag = 1;
		// 	ak_print_normal_ex("================ak_adec_notice_stream_end = NULL,exit =======================\n");
		// 	break;
		// }
		if (NULL != stream)
		{
			ak_demux_free_data(stream);
			stream = NULL;
		}

		ak_sleep_ms(10);
	}
	if (decode_close_flag == 0)
	{
		/* read over the file, must call ak_adec_read_end */
		ak_adec_notice_stream_end(play_ctrl.adec_handle);
		decode_close_flag = 1;
	}
	// yc_print_timeyc_print_time("退出read_record_file_and_play");
	wait_play_finished(play_ctrl.ao_handle);
	// yc_print_time("退出wait_play_finished");
	if (NULL != stream)
	{
		ak_demux_free_data(stream);
		stream = NULL;
	}
	/* read over the file, must call ak_adec_read_end */
	yc_print_time("%s exit\n", __func__);
	return ret;
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
static void save_volume_to_mac(int volume_value)
{
	char buf[6] = {0};
	sprintf(buf, "%d", volume_value);
	mac_set(buf);
	pr("保存音量到mac：%d\n", volume_value);
}

void voice_up()
{
	// yc_print_time("开始调音\n");
	// #ifdef FLASH_SAVE_VOLUME
	// 	char buf[6] = {0};
	// 	int volume_value = fha_mac_CheckMTD(PARTITION_MAC);
	// 	// yc_print_time("存完音量\n");
	// 	volume_value++;
	// 	// pr("声音增加，音量:%d\n", volume_value);
	// 	if (volume_value > VOICE_MAX_VOLUME)
	// 	{
	// 		volume_value = VOICE_MAX_VOLUME;
	// 	}
	// 	sprintf(buf, "%d", volume_value);

	// 	voice_set_volume(volume_value);
	// 	if (!voice_is_playing())
	// 	{
	// 		yc_interaction_func(STATUS_VOICE_CHANGE);
	// 	}
	// 	mac_set(buf);
	// 	// yc_print_time("设置完音量\n");
	// #else
	struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
	globle_config->volume++;
	ak_print_normal("声音增加，音量:%d\n", globle_config->volume);
	if (globle_config->volume > VOICE_MAX_VOLUME)
	{
		globle_config->volume = VOICE_MAX_VOLUME;
	}
	ak_config_set_system_user(globle_config);
	voice_set_volume(globle_config->volume);
	if (!voice_is_playing())
	{
		yc_interaction_func(STATUS_VOICE_CHANGE);
	}
	save_volume_to_mac(globle_config->volume);
	// #endif

	// yc_print_time("发送完提示音\n");
}

void voice_set(int volume)
{
	if (volume > VOICE_MAX_VOLUME)
	{
		volume = VOICE_MAX_VOLUME;
	}
	else if (volume < VOICE_MIN_VOLUME)
	{
		volume = VOICE_MIN_VOLUME;
	}
	struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
	globle_config->volume = volume;
	ak_config_set_system_user(globle_config);
	voice_set_volume(volume);
	if (!voice_is_playing())
	{
		yc_interaction_func(STATUS_VOICE_CHANGE);
	}
}

void voice_down()
{
	// #ifdef FLASH_SAVE_VOLUME
	// 	char buf[6] = {0};
	// 	int volume_value = fha_mac_CheckMTD(PARTITION_MAC);
	// 	volume_value--;
	// 	ak_print_normal("声音降低，音量:%d\n", volume_value);
	// 	if (volume_value < VOICE_MIN_VOLUME)
	// 	{
	// 		volume_value = VOICE_MIN_VOLUME;
	// 	}

	// 	voice_set_volume(volume_value);
	// 	if (!voice_is_playing())
	// 	{
	// 		yc_interaction_func(STATUS_VOICE_CHANGE);
	// 	}
	// char buf[6] = {0};
	// 	sprintf(buf, "%d", volume_value);
	// 	mac_set(buf);
	// #else
	struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
	globle_config->volume--;
	ak_print_normal("声音降低，音量:%d\n", (globle_config->volume));
	if (globle_config->volume < VOICE_MIN_VOLUME)
	{
		globle_config->volume = VOICE_MIN_VOLUME;
	}
	ak_config_set_system_user(globle_config);
	voice_set_volume(globle_config->volume);
	if (!voice_is_playing())
	{
		yc_interaction_func(STATUS_VOICE_CHANGE);
	}
	save_volume_to_mac(globle_config->volume);
	// #endif
}

static void close_demux_handle()
{
	if (NULL != play_ctrl.demux_handle)
	{
		pr("关闭ak_demux_close---\n");
		ak_demux_close(play_ctrl.demux_handle);
		play_ctrl.demux_handle = NULL;
	}
}

static void close_ao_handler()
{
	if (NULL != play_ctrl.ao_handle)
	{
		// voice_set_volume(1);
		/* ao disable speaker */
		pr("关闭ao---\n");
		//ak_ao_set_volume(play_ctrl.ao_handle, 1);
		ak_ao_enable_speaker(play_ctrl.ao_handle, AUDIO_FUNC_DISABLE);
		/* ao close */
		ak_ao_close(play_ctrl.ao_handle);
		play_ctrl.ao_handle = NULL;
#if defined YC_RTV || defined YC_XJ || defined YC_OWL
		uart_spkeak_disable();
#endif
	}
}

static void close_adec_handle()
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

	//if (play_ctrl.stop_flag)
	//{
	//	pr("音频已关闭\n");
	//	return;
	//}

	if (!play_ctrl.run_flag)
	{
		pr("uninit\n");
		return;
	}
	yc_print_time("开始停止音频");
	play_ctrl.stop_flag = 1;
	play_ctrl.pause_flag = 0;
	play_story_end_flag = 0;
	mqtt_stop_play_flag(0);

	struct audio_node *entry = NULL;
	struct audio_node *ptr = NULL;

	pr("%s开始清空链表\n", __func__);
	ak_thread_mutex_lock(&play_ctrl.play_mutex);
	list_for_each_entry_safe(entry, ptr, &(play_ctrl.file_list), list)
	{
		list_del_init(&(entry->list));
		pr("删除音频:%s，free_addr=%p\n", entry->file_name, entry);
		ak_free(entry);
		entry = NULL;
	}
	ak_thread_mutex_unlock(&play_ctrl.play_mutex);
	pr("%s结束清空链表\n", __func__);
	// close_demux_handle();
	// close_adec_handle();
	// close_ao_handler();
}

int voice_add_play_range(const char *file_name, unsigned long start_ms, unsigned long end_ms, int is_pause_play)
{
	// if(file_name)
	if (!play_ctrl.run_flag)
	{
		pr("uninitial\n");
		return AK_FAILED;
	}
	pr("adding file: %s\n", file_name);
	yc_check_mp3_is_warning(file_name);
	// struct audio_node *f = (struct audio_node *)ak_calloc(1, sizeof(struct audio_node));
	struct audio_node *f = (struct audio_node *)malloc(sizeof(struct audio_node));
	if (!f)
	{
		ak_print_error_ex("no memory\n");
		return AK_FAILED;
	}

	strncpy(f->file_name, file_name, FILE_NAME_LEN);
	f->n_ai_cb = play_ctrl.ai_cb;
	play_ctrl.ai_cb = NULL;
	pr("----add---startTimt:%lu,endTime：%lu\n", start_ms, end_ms);
	f->start_ms = start_ms;
	f->end_ms = end_ms;
	f->is_pause_play = is_pause_play;
	yc_print_time("添加音频");
	ak_thread_mutex_lock(&play_ctrl.play_mutex);
	list_add_tail(&f->list, &play_ctrl.file_list);
	ak_thread_mutex_unlock(&play_ctrl.play_mutex);
	ak_thread_sem_post(&play_ctrl.play_sem);

	return AK_SUCCESS;
}

int voice_add_play(const char *file_name)
{
	return voice_add_play_range(file_name, 0, 0, 0);
}

int voice_add_play_ai(const char *file_name, FUNC cb)
{
	if (ufo_check_machine_locked())
		return 0;
	play_ctrl.ai_cb = cb;
	voice_add_play(file_name);
	return 1;
}
/*node 在 list里面则返回1，否则返回0*/
static int check_node_is_list(struct audio_node *node)
{
	if (NULL == node)
	{
		return 0;
	}
	//return 0;
	struct audio_node *pos = NULL;
	if (list_empty(&play_ctrl.file_list))
	{
		//pr("list is null===================================\n");
		return 0;
	}

	list_for_each_entry(pos, &play_ctrl.file_list, list)
	{
		//pr("pos = %p==================================\n",pos);
		if (pos == node)
		{
			return 1;
		}
	}
	return 0;
}

static void release_node(struct audio_node *node)
{
	if (node != NULL)
	{
		ak_thread_mutex_lock(&play_ctrl.play_mutex);
		if (check_node_is_list(node))
		{
			list_del_init(&node->list);
			ak_free(node);
			node = NULL;
		}
		ak_thread_mutex_unlock(&play_ctrl.play_mutex);
	}
}

int yc_open_ao(struct ak_demux_media_info media_info)
{
	/* ao open */
	struct pcm_param param;
	param.sample_rate = media_info.audio_info.sample_rate;
	param.sample_bits = media_info.audio_info.sample_bits;
	param.channel_num = media_info.audio_info.channel_num;

	pr("打开demux 成功，将要打开ao \n");
	play_ctrl.ao_handle = ak_ao_open(&param);
	if (NULL == play_ctrl.ao_handle)
	{
		return 0;
	}

	/* enable speaker must set before ak_ao_send_frame */
	ak_ao_enable_speaker(play_ctrl.ao_handle, AUDIO_FUNC_ENABLE);

	/* volume is from 0 to 12,volume 0 is mute */
	// ak_ao_set_volume(play_ctrl.ao_handle, 4);
	// #ifdef FLASH_SAVE_VOLUME
	// 	int volume_value = fha_mac_CheckMTD(PARTITION_MAC);
	// 	voice_set_volume(volume_value);
	// #else
	struct sys_user_config *globle_config = (struct sys_user_config *)ak_config_get_system_user();
	voice_set_volume(globle_config->volume);
	// #endif
	/* ak_ao_set_resample have to use before ak_ao_send_frame */
	ak_ao_set_resample(play_ctrl.ao_handle, AUDIO_FUNC_DISABLE);

	/* before ak_ao_send_frame,must clear frame buffer */
	ak_ao_clear_frame_buffer(play_ctrl.ao_handle);
#if defined YC_RTV || defined YC_XJ || defined YC_OWL
	uart_spkeak_enable();
#endif
	return 1;
}
int yc_open_adec(struct ak_demux_media_info media_info)
{

	struct audio_param adec_param = {0};
	adec_param.type = media_info.audio_info.type;

	/* sample bits only support 16 bit */
	adec_param.sample_bits = 16;

	/* driver set to AUDIO_CHANNEL_MONO */
	adec_param.channel_num = media_info.audio_info.channel_num;
	adec_param.sample_rate = media_info.audio_info.sample_rate;

	/* open adec */
	play_ctrl.adec_handle = ak_adec_open(&adec_param);
	if (NULL == play_ctrl.adec_handle)
	{

		return 0;
	}
	return 1;
}

int yc_open_local_demux(char *file_name, struct ak_demux_media_info *media_info)
{
	pr("打开本地demux---\n");
	play_ctrl.demux_handle = ak_demux_open((char *)file_name, 0);
	if (play_ctrl.demux_handle == NULL)
	{
		ak_print_error_ex("demux open %s failed\n", file_name);
		return 0;
	}

	if (ak_demux_get_media_info(play_ctrl.demux_handle, media_info))
	{
		ak_print_error_ex("demux open %s failed\n", file_name);

		return 0;
	}
	return 1;
}
int create_http_socket(char *domain)
{
	int client_sockfd = 0;
	struct hostent *hp;
	struct in_addr in;
	int sinsize = sizeof(struct sockaddr_in);
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(struct sockaddr_in));

	if (!(hp = gethostbyname(domain)))
	{
		ak_print_error_ex("Can't not resolve server[%s]\n", domain);
		return -1;
	}
	else
	{
		memcpy(&server_address.sin_addr.s_addr, hp->h_addr, 4);
		in.s_addr = server_address.sin_addr.s_addr;
		ak_print_normal_ex("Server[%s] IP is :[%s]\n", domain, inet_ntoa(in));
	}

	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sockfd == -1)
	{
		ak_print_error_ex("fail to create TCP socket.\n");
		return -1;
	}
	struct timeval send_tv;
	send_tv.tv_sec = 2;
	send_tv.tv_usec = 0;
	// pr("设置时间戳秒：%lu   微秒：%lu  buf:%p\n", send_tv.tv_sec, send_tv.tv_usec);

	setsockopt(client_sockfd, SOL_SOCKET, SO_RCVTIMEO, &send_tv, sizeof(send_tv));
	setsockopt(client_sockfd, SOL_SOCKET, SO_SNDTIMEO, &send_tv, sizeof(send_tv));

	/* do connect */
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(CMD_SERVERD_TCP_PORT);

	int ret = connect(client_sockfd, (struct sockaddr *)&server_address,
		        (socklen_t)sinsize);
	if (ret)
	{
		ak_print_error_ex("connect: %s\n", strerror(errno));
		return -1;
	}

	return client_sockfd;
}

/**
 * decode_and_play - send the stream data to decode
 * @adec_handle[IN]: adec handle
 * @data[IN]: audio file data
 * @read_len[IN]: audio file data length
 * notes:
 */
static void decode_and_play(void *adec_handle, unsigned char *data, int read_len)
{
	int send_len = 0;
	int dec_len = 0;

	while (read_len > 0 && !play_ctrl.stop_flag)
	{
		/* send stream to decode */
		dec_len = ak_adec_send_stream(adec_handle, &data[send_len], read_len, 0);
		if (dec_len < 0)
		{
			ak_print_error_ex("write pcm to DA error!\n");
			break;
		}
		else
		{
			// ak_print_normal_ex("write [%d] byte to DA\n", dec_len);
		}
		read_len -= dec_len;
		send_len += dec_len;
	}
}

int yc_stream_play(char *url)
{
	int ret = NORNAL_EXIT;
	int content_ret = 0;
	if (url == NULL || strlen(url) == 0)
	{
		pr("流媒体播放给我个空的URL，播毛哦！\n");
		return ret;
	}

	int client_sockfd = 0;

	// http://turing-iot.oss-cn-beijing.aliyuncs.com/audio/nlp-b2e5923a8a0d40c492819f17472dfd5c-8420a211db3d4560b3b4f59eacc30d90.mp3
	char domain[128] = {0};
	char path[256] = {0};
	char *p1 = NULL, *p2 = NULL;
	p1 = url + 7;
	p2 = strstr(p1, "/");
	if (p2 == NULL)
	{
		pr("url格式不对啊：%s\n", url);
		return ret;
	}
	strncpy(domain, p1, p2 - p1);
	strncpy(path, p2, url + strlen(url) - p2);
	pr("分解出来域名：%s,路径：%s\n", domain, path);
	/* create socket server */
	client_sockfd = create_http_socket(domain);
	if (client_sockfd < 0)
	{
		pr("create http client socket error");
		return ret;
	}
	char send_msg[10240] = {0};
	sprintf(send_msg, HTTP_MSG, path, domain);
	pr("发送信息：%s\n", send_msg);
	/* send msg to server to get media stream */
	content_ret = send(client_sockfd, send_msg, strlen(send_msg), 0);
	if (content_ret < 0)
	{
		pr("send msg to server error[%s] \n", strerror(errno));
		return ret;
	}

	/*try to read data from socket */
	unsigned char buf[BUF_LEN];
	unsigned char *media_pt = NULL;
	struct stream_data adec_data;
	int read_len = -1;
	int tag_len = 0;
	int total_len = 0;
	int data_len = 0;
	int media_len = 0;
	int play_ts = 0;
	int media_play_end = 0;

	/***************************
		step 1: read data to buffer from socket to parse media info 
	***************************/

	/*read data from internet server*/
	memset(buf, 0, BUF_LEN);
	int i = 5;
	while (read_len < 0 && i > 0)
	{
		read_len = recv(client_sockfd, buf, BUF_LEN, 0);
		i--;
		pr("获取长度：%d，次数：%d\n", read_len, i);
		if (read_len < 0)
			usleep(50000);
	}

	pr("收到头数据:%s\n", buf);
	/* should ignore the httpd header, only for HTTP protocol*/
	while (read_len > 0 && !play_ctrl.stop_flag)
	{
		char *pt = strstr((char *)buf, "Content-Length: ");
		/*get the media data length info*/
		if (pt)
		{
			char tmp[32];
			char *tail = strstr(pt, "\r\n");
			int len = strlen("Content-Length: ");
			memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, pt + len, tail - pt - len);
			media_len = atoi(tmp);
			pr("the media file is [%d] bytes!\n", media_len);
		}

		/* check whether buf already recv the end lable of http header*/
		pt = strstr((char *)buf, "\r\n\r\n");
		if (pt != NULL)
		{
			/* ignore the http header*/
			data_len = read_len - ((unsigned char *)pt - buf + 4);
			memcpy(buf, pt + 4, data_len);
			total_len += data_len;
			read_len = 0;
			break;
		}
		// ak_print_normal_ex("buf is:[%s]\n", buf);
		memset(buf, 0, sizeof(buf));
		read_len = recv(client_sockfd, buf, sizeof(buf), 0);
	}
	read_len = -1;
	/*read the rest part of media data*/
	memset(&buf[data_len], 0, sizeof(buf) - data_len);
	pr("media_len:%d----date_len:%d\n", media_len, data_len);
	i = 5;
	while (media_len > 0 && media_len > data_len && i > 0 && read_len <= 0)
	{
		read_len = recv(client_sockfd, &buf[data_len], sizeof(buf) - data_len, 0);
		if (read_len > 0)
		{
			total_len += read_len;
		}
		else
		{
			usleep(50000);
		}

		i--;
	}
	read_len += data_len;

	if (read_len < 0)
	{
		pr("Client recv msg from server error!\n");
		return ret;
	}
	else
	{
		pr("Client successfully recv [%d] bytes data from server!\n", read_len);
	}

	/* check whether midia data carried tags in the header*/
	if (memcmp(buf, "ID3", 3) == 0)
	{
		tag_len = ((buf[6] & 0x7f) << 21) + ((buf[7] & 0x7f) << 14) + ((buf[8] & 0x7f) << 7) + (buf[9] & 0x7f) + 10;
		// ak_print_normal_ex("tag_len is:0x[%x]\n", tag_len);

		/* media data carried tags info, should ignore the tags info*/
		while (tag_len > 0 && !play_ctrl.stop_flag)
		{
			int buf_len = sizeof(buf);
			int tmp_len = 0;
			if (tag_len >= read_len)
			{
				tag_len -= read_len;
				memset(buf, 0, sizeof(buf));
				read_len = 0;
			}
			else
			{
				/* get the media data len*/
				read_len = read_len - tag_len;
				/*copy the media data to the begin of buf*/
				memcpy(buf, &buf[tag_len], read_len);
				/*reset the rest buf*/
				memset(&buf[read_len], 0, buf_len - read_len);
				tag_len = 0;
			}
			// ak_print_normal_ex("Client need to read [%d] bytes more data from server!\n", buf_len - read_len);
			tmp_len = recv(client_sockfd, &buf[read_len], buf_len - read_len, 0);
			read_len += tmp_len;
			total_len += tmp_len;
		}
	}

	/* set the media data info to struct for parse*/
	memset(&adec_data, 0, sizeof(struct stream_data));
	adec_data.data = buf;
	adec_data.len = read_len;
	adec_data.cur = 0;
	// ak_print_normal_ex("Client final read [%d] bytes media data from server to parse media info!\n", read_len);

	/***************************
		step 2: get media information 
	***************************/

	void *demux_handle = ak_demux_open_by_stream((void *)&adec_data, 0);
	if (!demux_handle)
	{
		pr("demux open failed\n");
		return ret;
	}
	else
	{
		pr("ak_demux_open_by_type successfule!\n");
	}

	struct ak_demux_media_info media_info;
	int dumex_ret = ak_demux_get_media_info(demux_handle, &media_info);
	if (dumex_ret)
	{
		pr("demux get info failed\n");
		return ret;
	}
	else
	{
		// ak_print_normal_ex("parse file info:type[%d],sample_rate[%d],sample_bits[%d],channel[%d],time[%d]\n",
		//          media_info.audio_info.type, media_info.audio_info.sample_rate, media_info.audio_info.sample_bits, media_info.audio_info.channel_num, media_info.totaltime_ms);
	}

	/***************************
		step 3: open and init ao 
	***************************/

	if (!yc_open_ao(media_info))
	{
		goto ao_end;
	}
	/***************************
			step 3: open adec
		***************************/
	// struct audio_param adec_param = {0};
	// adec_param.type = media_info.audio_info.type;

	// /* sample bits only support 16 bit */
	// adec_param.sample_bits = 16;

	// /* driver set to AUDIO_CHANNEL_MONO */
	// adec_param.channel_num = media_info.audio_info.channel_num;
	// adec_param.sample_rate = media_info.audio_info.sample_rate;
	if (!yc_open_adec(media_info))
	{
		goto adec_end;
	}

	// /* bind audio output & decode, request stream */
	// stream_handle = ak_adec_request_stream(play_ctrl.ao_handle, play_ctrl.adec_handle);

	// if (NULL == stream_handle)
	// {
	// 	ak_print_normal("\t ak_adec_request_stream failed! \n");
	// 	goto adec_end;
	// }

	/***************************
		step 4: receive the media stream and play
	***************************/
	pr("begin to play the media stream!\n");
	/* amr file,have to skip the head 6 byte */
	if (AK_AUDIO_TYPE_AMR == media_info.audio_info.type)
	{
		media_pt = &buf[6];
	}
	else
	{
		media_pt = buf;
	}

	while (!play_ctrl.stop_flag)
	{

		if (read_len > 0)
		{
			// pr("send [%d]bytes to decoder\n", read_len);
			decode_and_play(play_ctrl.adec_handle, media_pt, read_len);
			read_len = 0;
		}

		/* already receive all the media file, should stop decode*/
		if (media_len > 0 && total_len >= media_len)
		{
			if (decode_close_flag == 0)
			{
				/* read over the file, must call ak_adec_read_end */
				ak_adec_notice_stream_end(play_ctrl.adec_handle);
				decode_close_flag = 1;
			}
			media_play_end++;
		}

		int frame_len = get_pcm_frame_and_play(play_ctrl.adec_handle, play_ctrl.ao_handle);
		// ak_print_normal_ex("get and play [%d] bytes!\n",frame_len);
#if 0 // 由于PCM波特率与本地播放波特率不一样，以时间戳作为退出不准确
		if (frame_len > 0)
		{
			int play_interval = get_play_pcm_interval(&media_info, frame_len);
			play_ts += play_interval;
			if (play_ts > media_info.totaltime_ms)
				break;
		}
#endif
		if (no_frame_count > 3)
			break;

		if (media_play_end > 0)
		{
			break;
		}

		//print_playing_dot();
		if (media_len > 0 && total_len < media_len && !play_ctrl.stop_flag)
		{
			memset(buf, 0, sizeof(buf));
			read_len = recv(client_sockfd, buf, sizeof(buf), 0);
			if (read_len > 0)
				total_len += read_len;
			media_pt = buf;
		}
		// pr("read [%d] bytes from server,total len is[%d]!\n", read_len, total_len);
	}

	if (decode_close_flag == 0)
	{
		/* read over the file, must call ak_adec_read_end */
		ak_adec_notice_stream_end(play_ctrl.adec_handle);
		decode_close_flag = 1;
	}
	pr("Client read data finish, total len is[%d]!\n", total_len);

	/* decode end, cancel stream */
	// ak_adec_cancel_stream(stream_handle);
	// stream_handle = NULL;
	wait_play_finished(play_ctrl.ao_handle);
adec_end:
	close_adec_handle();
ao_end:
	close_ao_handler();
	ak_demux_close_by_stream(demux_handle);
	if (client_sockfd != -1)
		close(client_sockfd);
	if (play_ctrl.stop_flag != 0)
	{
		ret = STOP_EXIT;
	}
	pr("-----  exit ----- ret:%d\n", ret);

	return ret;
}

int static yc_is_stream_url(char *file_name)
{
	if (strstr(file_name, "http") != NULL)
	{
		return 1;
	}
	return 0;
}
static void copy_for_dual_channel(const unsigned char *src, int len,
			    unsigned char *dest)
{
	int j = 0;
	int count = (len / 2);

	for (j = 0; j < count; ++j)
	{
		dest[j * 4] = src[j * 2];
		dest[j * 4 + 1] = src[j * 2 + 1];
		dest[j * 4 + 2] = src[j * 2];
		dest[j * 4 + 3] = src[j * 2 + 1];
	}
}
static int yc_wav_play(char *file_name)
{
	pr("播放wav提示音\n");
	int channel_num = 1;
	/* open pcm fle */
	FILE *fp = fopen(file_name, "r");
	int ret = STOP_EXIT;
	if (NULL == fp)
	{
		pr("音频文件不存在：%s\n", file_name);
		return STOP_EXIT;
	}
	//跳过字节头
	fseek(fp, 44, SEEK_SET);
	struct ak_demux_media_info media_info = {0};
	/* sample bits only support 16 bit */
	media_info.audio_info.sample_bits = 16;
	/* driver set to AUDIO_CHANNEL_STEREO */
	media_info.audio_info.channel_num = channel_num;
	/* get sample rate */
	media_info.audio_info.sample_rate = 16000;
	if (!yc_open_ao(media_info))
	{
		pr("打开ao失败：%s\n", file_name);
		return STOP_EXIT;
	}
	int read_len = 0;
	int send_len = 0;
	int total_len = 0;
	unsigned char data[4096] = {0};
	unsigned char full_buf[4096 * 2];
	while (play_ctrl.stop_flag == 0)
	{
		/* read the pcm file data */
		memset(data, 0x00, sizeof(data));
		/* read pcm data */
		read_len = fread(data, sizeof(char), sizeof(data), fp);

		if (read_len > 0)
		{
			total_len += read_len;
			switch (channel_num)
			{
			case AUDIO_CHANNEL_MONO:
				copy_for_dual_channel(data, read_len, full_buf);
				send_len = (read_len << 1);
				break;
			case AUDIO_CHANNEL_STEREO:
				memcpy(full_buf, data, read_len);
				send_len = read_len;
				break;
			default:
				return STOP_EXIT;
			}

			/* send frame and play */
			if (ak_ao_send_frame(play_ctrl.ao_handle, full_buf, send_len, 0) < 0)
			{
				ak_print_error_ex("write pcm to DA error!\n");
				break;
			}

			// print_playing_dot();
			ak_sleep_ms(10);
		}
		else if (0 == read_len)
		{
			ak_ao_notice_frame_end(play_ctrl.ao_handle);
			/* read to the end of file */
			ak_print_normal("\n\t read to the end of file\n");

			break;
		}
		else
		{
			ak_print_error("read, %s\n", strerror(errno));
			break;
		}
	}

	/* wait the driver play end */
	if (0 == read_len)
	{
		wait_play_finished(play_ctrl.ao_handle);
	}
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}

	close_ao_handler();
	ak_print_normal("%s exit\n", __func__);
	if (play_ctrl.stop_flag == 0)
	{
		ret = NORNAL_EXIT;
	}
	// yc_print_time("播完提示音\n");
	return ret;
}

static void *play_work_th(void *arg)
{
	int play_exit_value;
	char file_name_buf[256];
	FUNC ai_cb_buf = NULL;
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
			pr("空节点，释放锁\n");
			release_node(node);
			play_ctrl.stop_flag = 1;
			continue;
		}

		strcpy(file_name_buf, node->file_name);
		ai_cb_buf = node->n_ai_cb;
		decode_close_flag = 0;
		if (strstr(node->file_name, "/wav/") != NULL)
		{
			//播wav音频
			play_exit_value = yc_wav_play(node->file_name);
		}
		else if (yc_is_stream_url(node->file_name))
		{
			pr("播放流媒体：%s\n", node->file_name);
			play_exit_value = yc_stream_play(node->file_name);
			if (-1 == play_exit_value)
			{
				pr("加载流媒体失败，将再试一次\n");
				play_exit_value = yc_stream_play(node->file_name);
			}
		}
		else
		{

			struct ak_demux_media_info media_info;

			if (!yc_open_local_demux(node->file_name, &media_info))
			{
				close_demux_handle();
				release_node(node);
				play_ctrl.stop_flag = 1;
				continue;
			}

			pr("媒体时长ms=%d\n", media_info.totaltime_ms);
			if (!yc_open_ao(media_info))
			{
				ak_print_normal("\t ak_ao_open failed !\n");
				close_demux_handle();
				release_node(node);
				play_ctrl.stop_flag = 1;
				continue;
			}
			/***************************
			step 3: open adec
		    ***************************/
			if (!yc_open_adec(media_info))
			{
				ak_print_normal("\t ak_adec_open failed! \n");
				close_ao_handler();
				close_demux_handle();
				release_node(node);
				play_ctrl.stop_flag = 1;
				continue;
			}

			//播放故事机模式的UX
			if (strncmp(node->file_name, "/usr/share", strlen("/usr/share")))
			{
				yc_interaction_func(STATUS_VOICE_PLAY_START);
			}

			pr("进入播放循环\n");
			if (node->start_ms != node->end_ms)
			{
				if (0 == node->is_pause_play)
				{
					//绘本阅读一页播放完标志
					play_story_end_flag = 1;
				}
				play_ctrl.totaltime_ms = node->end_ms;
				play_exit_value = read_record_file_and_play(node->start_ms, node->end_ms, &media_info);
			}
			else
			{
				play_ctrl.totaltime_ms = media_info.totaltime_ms;
				play_exit_value = read_record_file_and_play(0, media_info.totaltime_ms, &media_info);
			}

			close_adec_handle();
			close_ao_handler();
			close_demux_handle();
			yc_print_time("退出播放循环\n");
		}
		// play_ctrl.stop_flag = 1;
		//退出播放有三种：1.正常播放结束 2.中途停止播放 3.暂停播放
		switch (play_exit_value)
		{
		case STOP_EXIT:
			yc_print_time("voice stop play , do nothing !\n");
			break;
		case PAUSE_EXIT:
			//保存暂停的音频时间总长，文件名
			play_ctrl.pause_totaltime_ms = play_ctrl.totaltime_ms;
			strncpy(play_ctrl.pause_file, file_name_buf, FILE_NAME_LEN);
			release_node(node);
			//处理完毕，通知pause可以解除阻塞
			play_ctrl.stop_flag = 1;
			yc_print_time("voice pause OK!\n");
			//continue;
			break;
		case NORNAL_EXIT:
			if (1 == play_ctrl.stop_flag)
			{
				pr("voice stop play after normal_exit,do nothing !\n");
				break;
			}
			play_ctrl.stop_flag = 1;
			//TODO:存在voice_stop_play 先free掉node ,然后node-> 就可能出现段错误
			yc_print_time("----- %s normal exit -----\n", file_name_buf);
			if (strncmp(file_name_buf, "/usr/share", strlen("/usr/share")))
			{
				yc_interaction_func(STATUS_VOICE_PLAY_STOP);
			}
			if (0 == strcmp(file_name_buf, "/usr/share/need_a_rest.mp3"))
			{
				yc_interaction_func(STATUS_VOICE_PLAY_STOP);
			}

			//检查是否需要回调处理
			if (ai_cb_buf != NULL)
			{
				ai_cb_buf(file_name_buf);
				if (!yc_is_stream_url(file_name_buf))
				{
					//绘本识图和本地文件，播放完了不删
					remove(file_name_buf);
					sync();
				}
			}
			//删除播放节点
			release_node(node);
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
			break;
		}
	}

	yc_print_time("exit ..., tid: %ld\n", tid);

	return NULL;
}

int voice_is_pause()
{
	return play_ctrl.pause_flag;
}

//TODO:有多个播放节点存在暂停的情况，sem_getvalue(),list_add增加节点stack...
void voice_pause()
{
	if (play_ctrl.stop_flag || play_ctrl.pause_flag)
	{
		pr("voice not need pause \n");
		return;
	}
	pr("voice will pause...\n");
	play_ctrl.pause_flag = 1;
	//线程挂起，直到暂停的线程执行完毕
	yc_interaction_func(STATUS_VOICE_PLAY_PAUSE);
	while (!play_ctrl.stop_flag)
	{
		ak_sleep_ms(1);
	}
}

void voice_resume()
{
	if (0 == play_ctrl.pause_flag)
	{
		pr("no voice playing \n");
		return;
	}
	pr("voice will resume play...\n");
	play_ctrl.pause_flag = 0;
	voice_add_play_range(play_ctrl.pause_file, play_ctrl.pause_ms, play_ctrl.pause_totaltime_ms, 1);
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

	ak_thread_mutex_destroy(&play_ctrl.play_mutex);
	ak_thread_sem_post(&play_ctrl.play_sem);
	if (play_ctrl.play_tid != 0)
	{
		ak_thread_join(play_ctrl.play_tid);
	}

	ak_thread_sem_destroy(&play_ctrl.play_sem);
	close_adec_handle();
	close_ao_handler();
	close_demux_handle();
	ak_print_normal_ex("exit ...\n");

	return 0;
}

static int yc_check_mp3_is_warning(const char *file_name)
{
	if (NULL != strstr(file_name, "/need_a_rest.") ||
	    NULL != strstr(file_name, "/none_action_warning_0.") ||
	    NULL != strstr(file_name, "/none_action_warning_1.") ||
	    NULL != strstr(file_name, "/recognize_again_tips_0.") ||
	    NULL != strstr(file_name, "/recognize_again_tips_1.") ||
	    NULL != strstr(file_name, "/recognize_again_tips_2."))
	{
		sleep_warning_flag = 1;
	}
	else
	{
		sleep_warning_flag = 0;
	}
	return sleep_warning_flag;
}

int voice_is_warning(void)
{
	return sleep_warning_flag;
}

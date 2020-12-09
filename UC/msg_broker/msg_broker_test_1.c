#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<strings.h>

#include<unistd.h>
#include<pthread.h>

#include"msg_broker.h"

#define IVS_MSG_TRIGGER        "ivsTrigger"
#define IVS_MSG_GET_FD_COUNT   "ivsGetFdCount"
#define IVS_MSG_GET_HBD_COUNT  "ivsGetHbdCount"
#define IVS_MSG_GET_MD_WINDOW  "ivsGetMDWindow"

#define IVS_HOST			"IVS_HOST"
#define IVS_MSG_ROI			"IVS_MSG_ROI"
#define IVS_MSG_USER		"IVS_MSG_USER"
#define IVS_MSG_DB			"IVS_MSG_DB"
#define IVS_MSG_RESET		"IVS_MSG_RESET"
#define IVS_MSG_OSD			"IVS_MSG_OSD"
#define IVS_MSG_FEC			"IVS_MSG_FEC"
#define IVS_MSG_SETUP		"IVS_MSG_SETUP"
#define IVS_MSG_MHBD		"IVS_MSG_MHBD"
#define IVS_MSG_ISP_MD		"IVS_MSG_ISP_MD"
#define IVS_MSG_TD			"IVS_MSG_TD"
#define IVS_MSG_MD			"IVS_MSG_MD"
#define IVS_MSG_OM			"IVS_MSG_OM"
#define IVS_MSG_OT			"IVS_MSG_OT"
#define IVS_MSG_ENCODE		"IVS_MSG_ENCODE"
#define IVS_MSG_FD_MASK		"IVS_MSG_FD_MASK"
#define IVS_MSG_BD_MASK		"IVS_MSG_BD_MASK"

#define VENC_CMD_FIFO       "/tmp/venc/c0/command.fifo"
#define VENC_IVS_CMD_FIFO   "/tmp/venc/c1/command.fifo"

pthread_t tid_cli,tid_srv;

void msg_ivs_callback(MsgContext* msg_context, void* user_data __attribute__((__unused__)))
{
	if (!strcasecmp(msg_context->pszHost, IVS_HOST)){

		if (!strcmp(msg_context->pszCmd, IVS_MSG_SETUP)){
			printf("[%s:%d]pszHost = %s,pszCmd=%s,pbyData=%d\n",__func__,__LINE__,msg_context->pszHost,msg_context->pszCmd,*((int*)msg_context->pbyData));
		} 
		else if (!strcmp(msg_context->pszCmd, IVS_MSG_TD)){
			printf("[%s:%d]pszHost = %s,pszCmd=%s,pbyData=%d\n",__func__,__LINE__,msg_context->pszHost,msg_context->pszCmd,*((int*)msg_context->pbyData));
		}
		else if (!strcmp(msg_context->pszCmd, IVS_MSG_GET_MD_WINDOW)){
			printf("[%s:%d]pszHost = %s,pszCmd=%s,pbyData=%d\n",__func__,__LINE__,msg_context->pszHost,msg_context->pszCmd,*((int*)msg_context->pbyData));
		} 
	}
	else{
		printf("[%s:%d]pszHost = %s,pszCmd=%s,pbyData=%d\n",__func__,__LINE__,msg_context->pszHost,msg_context->pszCmd,*((int*)msg_context->pbyData));
	}
	
	if (msg_context->bHasResponse) {
		msg_context->dwDataSize = 0;
	}
}

static int bTerminate = 0;
static void srv_thread(void* param){
	printf("%s pthread id = %lu\n", __func__, pthread_self());
	pthread_detach(pthread_self());
	int srv_flag =0;
	while(1){
		//服务端建立
		if(!srv_flag){
			srv_flag = 1;
			MsgBroker_RegisterMsg(VENC_CMD_FIFO); 
			MsgBroker_Run(VENC_CMD_FIFO, msg_ivs_callback, NULL, &bTerminate);
			MsgBroker_UnRegisterMsg();
			printf("%s:%d---------------------here------------------\n",__func__,__LINE__);
		}
		sleep(1);
	}
	
	
}

static void cli_msg(const char* pszHost,const char* pszCmd,unsigned int value ){
		MsgContext msg_context;
		
		msg_context.pszHost = pszHost;
		msg_context.dwHostLen = strlen(msg_context.pszHost) + 1; 
		msg_context.pszCmd = pszCmd;
		msg_context.dwCmdLen = strlen(msg_context.pszCmd) + 1;
		msg_context.dwDataSize = sizeof(unsigned int);
		msg_context.pbyData = (uint8_t*) &value;
		msg_context.bHasResponse = 0;
		if(MsgBroker_SendMsg(VENC_IVS_CMD_FIFO, &msg_context) == -1){
			printf("MsgBroker_SendMsg Fail!\n");
		}
}

static void cli_thread(void* param){
	printf("%s pthread id = %lu\n", __func__, pthread_self());
	pthread_detach(pthread_self());
	while(1){
		int rand_val = rand() % 4;
		switch(rand_val){
			case 0:
				cli_msg(IVS_HOST,IVS_MSG_SETUP,rand_val);
			break;
			case 1:
				cli_msg(IVS_MSG_TRIGGER,IVS_MSG_GET_FD_COUNT,rand_val);
			break;
			case 2:
				cli_msg( IVS_MSG_TRIGGER,IVS_MSG_GET_HBD_COUNT,rand_val);
			break;
			case 3:
				cli_msg( IVS_MSG_TRIGGER,IVS_MSG_GET_MD_WINDOW,rand_val);
			break;
			default:
			break;
		}
		sleep(2);
	}
}


int main(){
	
	//种下随机种子
	srand(time(0));
	
	//创建服务线程
	pthread_create(&tid_srv, NULL, (void *)&srv_thread, NULL);
	
	//创建客户线程
	pthread_create(&tid_srv, NULL, (void *)&cli_thread, NULL);
	while(1){
		sleep(1);
	}
	return 0;
}
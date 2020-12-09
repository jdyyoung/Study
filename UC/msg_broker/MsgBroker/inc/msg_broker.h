
/*
 *******************************************************************************
 *  Copyright (c) 2010-2015 VATICS Inc. All rights reserved.
 *
 *  +-----------------------------------------------------------------+
 *  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
 *  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
 *  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
 *  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
 *  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
 *  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
 *  |                                                                 |
 *  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
 *  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
 *  | VATICS INC.                                                     |
 *  +-----------------------------------------------------------------+
 *
 *******************************************************************************
 */
#ifndef MSG_BROKER_H
#define MSG_BROKER_H

#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SR_MODULE_NAME	"daemon_sr"
#define SR_CMD_FIFO		"/tmp/sr/c0/command.fifo"
#define REGISTER_HOST	"process"
#define SUSPEND_CMD		"suspend"
#define SUSPEND_ACK		"suspend_ack"
#define RESUME_CMD		"resume"
#define REGISTER_CMD	"register"
#define UNREGISTER_CMD	"unregister"


typedef enum {
	STOP,
	START
}STATUS;

typedef struct register_node {
	pid_t process_pid;
	char cmdfifo[50];
	STATUS status;
}register_node;


/**
 * A Structure for message transmission between processes
 */
typedef struct
{
	unsigned int dwHostLen;    //! Length of pszHost
	const char* pszHost;       //! The pointer to host data

	unsigned int dwCmdLen;     //! Length of pszCmd
	const char* pszCmd;        //! The pointer to command data

	unsigned int dwDataSize;   //! Data size of pbyData
	unsigned char* pbyData;    //! The pointer to bytes of data you want to transmit, max size is 256 bytes;
	unsigned int bHasResponse; //! Sender need feadback or not, 0: no, 1: yes
} MsgContext;

/**
 * A function pointer for message callback function which was called in MsgBroker_Run
 */
typedef void (*MsgCallBack)(MsgContext*, void* user_data);

/**
 * @brief Function for message reciver to get message 
 *
 * @param[in] pszPath The path to command fifo.
 * @param[in] pfnFunc The function pointer for MsgCallBack.
 * @param[in] pUserData User data pointer to pass in MsgCallBack function.
 * @param[in] piTerminate The pointer to terminate flag to inform MsgBroker to quit message listening.
 * @return The handle of resize object
 */
void MsgBroker_Run(const char* pszPath, MsgCallBack pfnFunc, void* pUserData, int* piTerminate);

/**
 * @brief Function for message sender to send message 
 *
 * @param[in] pszPath The path to command fifo.
 * @param[in] ptContext The pointer to MsgContext. 
   @return Success: 0  Fail: negative integer.
 */
int MsgBroker_SendMsg(const char* pszPath, MsgContext* ptContext);

/**
 * @brief Function send register message to daemon_sr   
 *
 * @param[in] pszPath The path of own command fifo.
   @return Success: 0  Fail: negative integer.
 */
int MsgBroker_RegisterMsg(const char * fifo);

/**
 * @brief Function send unregister message to daemon_sr   
 *
   @return Success: 0  Fail: negative integer.
 */
int MsgBroker_UnRegisterMsg();

/**
 * @brief Function send suspend finish ack message to daemon_sr   
 *
   @return Success: 0  Fail: negative integer.
 */
int MsgBroker_SuspendAckMsg();

/**
 * @brief Function send suspend message to register app   
 *
 * @param[in] pszPath The path of own command fifo.
   @return Success: 0  Fail: negative integer.
 */
int MsgBroker_SuspendMsg(const char* cmdfifo);

/**
 * @brief Function send resume message to register app   
 *
 * @param[in] pszPath The path of own command fifo.
   @return Success: 0  Fail: negative integer.
 */
int MsgBroker_ResumeMsg(const char* cmdfifo);


#ifdef __cplusplus
}
#endif

#endif

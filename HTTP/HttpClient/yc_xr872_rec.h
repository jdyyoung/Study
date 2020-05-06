#ifndef _YC_XR872_REC_H_
#define _YC_XR872_REC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PICTURE_MAX_SIZE (36*1024)

enum yc_rec_status {
    YC_STATUS_OK           = 0, 

    /* error status */
    YC_STATUS_ERROR_MIN           = 120,
	YC_STATUS_IN_PARM_ERR_120     = 120,
	YC_STATUS_REPEAT_INIT_121     = 121,
	YC_STATUS_NO_INIT_122         = 122,
    YC_STATUS_MEM_ALLOC_FAIL_123  = 123,     /* memery alloctate fail */
	YC_STATUS_HTTP_POST_ERR_124   = 124,
    YC_STATUS_JSON_PARSE_ERR_125  = 125,     /* JSON no some key*/
	YC_STATUS_STR_PARSE_NULL_126  = 126,
	YC_STATUS_BASE64_ERR_127      = 127,
	YC_STATUS_404_NO_REC_PIC_128  = 128,  /*recognize server return 404 ,cannot recognize picture*/
    YC_STATUS_ERROR_MAX           = 128,
};

/* YC recognize running auth parms data struct  */
typedef struct auth_parm_t{
	char device_mode[5];
	char version[9];
	char sn[32];
	char mac[32];
	char api_key[29]; 
	char api_secret[65];
}auth_parm_t;

typedef  struct out_info_t{
	char* music_url;
	char* isbn_page;
	char* meta;
}out_info_t;

//memory
typedef void *(*yc_malloc_t)(size_t size);
typedef void (*yc_free_t)(void *ptr);
typedef void *(*yc_calloc_t)(unsigned long nmemb, unsigned long size);
typedef void *(*yc_realloc_t)(void *ptr, unsigned long size);

typedef struct yc_ops_t{
    yc_malloc_t yc_plat_malloc;
    yc_free_t yc_plat_free;
    yc_calloc_t yc_plat_calloc;
    yc_realloc_t yc_plat_realloc;
	void* yc_ops_reserve;
} yc_ops_t;

/**
 * @brief:初始化绘本识别
 * @param [in] yc_auth :认证所需参数信息
 * @param [in] ops_parm ：内存管理回调接口
 * @return：成功：0;失败：非0
 */
int yc_recognize_init(auth_parm_t* yc_auth,yc_ops_t* ops_parm);

/**
 * @brief:根据传入的图片数据，识别出结果，并返回music_url
 * @param  [in]jpg_data：图片数据地址
 * @param  [in]data_len：图片数据长度
 * @param  [out]out_data:返回的要获取的信息数据
 * @return 成功：0;失败：非0
 */
uint8_t yc_recognize_run(uint8_t* jpg_data,uint32_t data_len,out_info_t* out_data);

/**
 * @brief:销毁识别服务
 * @param  [in]none
 * @return：none
 */
void yc_recognize_deinit();

#ifdef __cplusplus
}
#endif

#endif //_YC_XR872_REC_H_
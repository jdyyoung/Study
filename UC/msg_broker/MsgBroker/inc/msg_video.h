#ifndef MSG_VIDEO_H
#define MSG_VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif


#define OSD_STR_SIZE 64

/*========== Video Encoder =========*/
typedef struct
{
	/* Common */
	unsigned int type;				// 0: H.264, 1: H.265, 2: MJPEG
	unsigned int w;					// encoding width
	unsigned int h;					// encoding height
	unsigned int fps;				// encoding fps
	unsigned int qp;				// qp

	unsigned int gop;				// gop
	unsigned int bitrate;			// bitrate
	unsigned int advanced_mode; 	/**< consider 0 -> fps priority, 1 - > quality priority, 2 -> customized */	
	unsigned int min_qp;			// for customized mode only
	unsigned int max_qp;			// for customized mode only
	/* H.264 only */
	unsigned int min_fps;			// for customized mode only
} msg_video_t;

typedef struct
{
	int 	output_index;
	char	str[OSD_STR_SIZE];		
} msg_osd_t;

typedef struct
{
	/* ROI window mode */
	unsigned int enable;		//! Enable ROI or not 
	unsigned int window_idx;	//! H.264: 0~7, H.265: 0~63
								//! Window index 7 in H.264 is background window. 
								//! Background window can not set position and it will cover all picture.
	/* H264: one macroblock is 16x16 pixels, H265: one macroblock is 64x64 pixels */
	unsigned int start_x;		//! The x start position of the macroblock 
	unsigned int start_y;		//! The y start position of the macroblock 
	unsigned int end_x;			//! The x end position of the macroblock 
	unsigned int end_y;			//! The y end position of the macroblock 
	
	int 		 qp;			//! Delta QP 
	unsigned int enc_interval;	//! Encoding Interval: ROI window in H.265 do not support interval parameter.
} msg_roi_t;

/**
 * @struct      msg_fec_t
 * @brief       The structure for the fisheye correction.
 */
typedef struct
{
	/* fisheye correction */
	int 	output_index;	/*!< The output index */
	int 	mode;				/*!< 0:ori, 1:p360, 2:p180_a, 3:p180_s, 4:p180_t, 5:6r, 6:4r, 7:1p2r, 8:1o3r, 9: sphere, 10:1r, 11:p360_s */
	float 	pan;			/*!< Pan (reserved): value range depends on de-warping mode, please refer to coeff_gen.h */
	float 	tilt;			/*!< Tilt (reserved): value range depends on de-warping mode, please refer to coeff_gen.h */
	float 	zoom;			/*!< Zoom (reserved) value range depends on de-warping mode, please refer to coeff_gen.h */
	float 	focal;			/*!< Focal: value range depends on de-warping mode, please refer to coeff_gen.h */
	
	int   	cell_idx;		/*!< Index of a certain partition of the layout, range: depends on layout */
	int   	step;     		/*!< The step count. -1 means infinite or positive integer */
	float 	pan_step;		/*!< The step unit of pan */
	float 	tilt_step;		/*!< The step unit of tilt */
	float 	zoom_step;		/*!< The step unit of zoom */
	float 	curvature;		/*!< The curvature */
	float 	slope;			/*!< The slope */
	
	int 	app_type;		/*!< The applied type: 0: ceiling, 1: table, 2: wall */

	int 	lens;			/*!< 0: LENS_STEREOGRAPHIC, 1: LENS_EQUISOLIDANGLE, 2: LENS_EQUIDISTANT, 3: LENS_ORTHOGRAPHIC, 5: LENS_NODISTORT */
	float 	dst_offset_x;	/*!< The destination offset in the x direction. */
	float 	dst_offset_y; 	/*!< The destination offset in the y direction. */	
	float 	dst_xy_ratio; 	/*!< The destination x/y ratio. Range: 0.1 ~ 2 */

	//int 	enable_object_tracking;
} msg_fec_t;

#ifdef __cplusplus
}
#endif

#endif
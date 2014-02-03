#ifndef __M6MO_REGS_H__ 
#define __M6MO_REGS_H__ 

/* 
  * category 0x00 registers, system parameter
*/
#define SYS_MODE_REG					0x000b   /* system mode */
enum sys_mode {
	SYS_INITIALIZE_MODE, /* this mode read only */
	SYS_PARAMETER_MODE,
	SYS_MONITOR_MODE,
	SYS_CAPTURE_MODE,
};

#define INT_ENABLE_REG					0x0010   /* interrupt enable */
#define INT_ROOR_ENABLE_REG			0x0012   /* root interrupt enable */
#define FIRMWARE_MINOR_VER_REG 		0x0015   /* firmware version, 16 bit */

#define INT_FACTOR_REG					0x001c   /*interrupt status*/
enum int_mask {
	INT_MASK_SOUND 		= 0x80,
	INT_MASK_LENS_INIT 	= 0x40,
	INT_MASK_FD 			= 0x20,
	INT_MASK_FRAMESYNC 	= 0x10,
	INT_MASK_CAPTURE 		= 0x08,
	INT_MASK_ZOOM 		= 0x04,
	INT_MASK_AF 			= 0x02,
	INT_MASK_MODE 		= 0x01,
};

/*
  * category 0x01 registers, monitor & still parameter A
*/
#define OUT_SELECT_REG		0x0100   /* output interface, 8 bit */
enum output_select {
	YUV_IF = 0,
	HDMI_IF = 1,
	MIPI_IF = 2,
};

#define MON_SIZE_REG					0x0101  /* monitor size, 8 bit */
enum mon_size {
	MON_QVGA = 0x09,  		/* 320x240 */
	MON_VGA = 0x17,	  	/* 640x480 */
	MON_QCIF = 0x05,		/* 176x144 */
	MON_CIF = 0x0e,			/* 352x288 */
	MON_854X640 = 0x1d, 	/* 854x640, mach our LCD */
	MON_SVGA = 0x1f,		/* 800x600 */
	MON_960X720 = 0x20, 	/* 960x720 */
	MON_720P = 0x21,		/* 1280x720 */
	MON_1216X912 = 0x36,	/* 1216x912, m040 default preview size */
	MON_1080P = 0x28,		/* 1920x1080 */
};

#define MON_FPS_REG 0x0102        /* isp frame rate, 8 bit */
enum mon_fps {
	MON_FPS_AUTO = 1,
	MON_FPS_30,
	MON_FPS_15,
	MON_FPS_12,
	MON_FPS_10,
	MON_FPS_7P5,
};

#define FSHD_EN_REG		0x0107   /* shading table control, 8 bit*/
enum fshd_enable{
	FSHD_OFF,
	FSHD_ON,
	FSHD_MANUAL,/*for debug*/	
};

#define IASTER_CLK_FOR_SENSOR_REG		0x013a/* clock control, 32 bit */

/*
  * category 0x02 register
*/
#define ZOOM_POSITOIN_REG					0x0201
#define ZOOM_CFIXB_REG						0x0209
#define ZOOM_CFIXR_REG						0x020a
#define COLOR_EFFECT_REG					0x020b
enum color_effect {
	COLOR_EFFECT_ON,
	COLOR_EFFECT_OFF,
	COLOR_EFFECT_FILM,	
};

#define SCENE_MODE_REG					0x0237   /*scene mode setting*/
enum scene_mode {
	SCENE_OFF,
	SCENE_AUTO,
	SCENE_PORTRAIT,
	SCENE_LANDSCAPE,
	SCENE_SPORT,
	SCENE_NIGHT,
	SCENE_SUNSET,
	SCENE_MARCO,
	SCENE_CHARACTER = 0x0b,
};

#define SPECIAL_MON_REG					0x0238  /*panorama mode setting, 8 bit*/
enum special_mon {
	SPECIAL_OFF,
	SPECIAL_VAHS,
	SPECIAL_PANORAMA,
};

#define MON_REVERSE_ISP_REG				0x0252
#define MON_MIRROR_ISP_REG				0x0253

/*
  * category 0x03 registers, exposure control
*/
#define AE_LOCK_REG						0x0300   /* auto expore lock, 8 bit*/

#define EVP_MODE_MON_REG 0x030a  /*sensor framerate, 8 bit*/
enum evp_mode_mon {
	EVP_MODE_AUTO, 
	EVP_MODE_30FPS,
	EVP_MODE_15FPS,
};

#define ISO_SEL_REG							0x0305
enum iso_sel {
	ISO_SEL_AUTO=0,
	ISO_SEL_50,
	ISO_SEL_100,
	ISO_SEL_200,
	ISO_SEL_400,
	ISO_SEL_800,
	ISO_SEL_1600,
	ISO_SEL_3200,
};

#define EV_BIAS_REG							0x0309
enum ev_bias {
	EV_M2 = 0x0a,
	EV_M_1_5 =	0x0F,
	EV_M1 = 0x14,
	EV_M_0_5 = 0x19,
	EV_00 = 0x1e,
	EV_P_0_5 = 0x23,
	EV_P1 = 0x28,
	EV_P_1_5 = 0x2D,
	EV_P2 = 0x32,
};

#define SCENE_EV_REG					0x031e /* 16bit */

/* light info */
#define INFO_LIGHT_REG	(0x0334)

/*
  * category 0x06 register
*/
#define AWB_LOCK_REG				0x0600 /* auto while balance lock, 8 bit */
#define AWB_MODE_REG				0x0602  /* awb operating mode select, 8 bit */
enum awb_mode {
	AWB_AUTO = 0x01,
	AWB_MANUAL,
	AWB_TRACKING,
};

#define AWB_MANUAL_REG			0x0603  /* awb manual mode, 8 bit */
enum awb_manual { 
	AWB_PROHIBITION,
	AWB_INCANDESCENT,
	AWB_FLUORESCENT_HIGH,
	AWB_FLUORESCENT_LOW,
	AWB_DAYLIGHT,
	AWB_CLOUDY,
	AWB_SHADE,
	AWB_HORIZON,
};

/*
  * category 0x06 register, exif register
*/
#define INFO_EXPTIME_NUMERATOR_REG		0x0700  /*32bit, exposure time*/
#define INFO_EXPTIME_DENUMINATOR_REG	0x0704  /*32bit, exposure time*/
#define INFO_TV_NUMERATOR_REG			0x0708  /*32bit, shutter speed value*/
#define INFO_TV_DENUMINATOR_REG			0x0708  /*32bit, shutter speed value*/
#define INFO_AV_NUMERATOR_REG			0x070c  /*32bit, aperture value*/
#define INFO_AV_DENUMINATOR_REG			0x0714  /*32bit, aperture value*/
#define INFO_BV_NUMERATOR_REG			0x0718  /*32bit, brightness value*/
#define INFO_BV_DENUMINATOR_REG			0x071c  /*32bit, brightness value*/
#define INFO_EBV_NUMERATOR_REG			0x0720  /*32bit, exposure bias value*/
#define INFO_EBV_DENUMINATOR_REG			0x0724  /*32bit, exposure bias value*/
#define INFO_ISO_REG						0x0728  /*32bit, ISO speed value*/
#define INFO_FLASH_REG						0x072a  /*32bit, FLASH*/
#define INFO_SDR_REG						0x072c  /*32bit, subject distance range*/
#define INFO_QVAL_REG						0x072e  /*32bit, QVALUE*/

/*
  * category 0x08 register, panorama
*/
#define PANO_CAP_READY_REG				0x0834
enum pano_cap_action {
	PANO_CAP_NOT_READY,
	PANO_CAP_READY_START,
	PANO_CAP_READY_STOP,
};

#define PANO_CAP_DIRECTION_REG	0x0835
#define PANO_OFFSETX_H_REG		0x0836
#define PANO_OFFSETX_L_REG		0x0837
#define PANO_OFFSETY_H_REG		0x0838
#define PANO_OFFSETY_L_REG		0x0839
#define PANO_CAP_INDEX_REG		0x083a
#define PANO_ERROR_NO_REG		0x083b
#define PANO_CTRL_REG				0x083c	

/*
  * category 0x09 register
*/
#define FACE_DETECT_CTL_REG		0x0900
enum face_detect {
	FACE_DETECT_ON = 0x11,
	FACE_DETECT_OFF = 0x10,	
};
#define FACE_DETECT_MAX_REG		0x0902
#define FACE_DETECT_NUM_REG		0x090a
#define FACE_DETECT_READ_SEL_REG	0x090b
#define FACE_DETECT_X_LOCATION_REG 	0x090e
#define FACE_DETECT_Y_LOCATION_REG 	0x0910
#define FACE_DETECT_FRAME_WIDTH_REG	0x0912
#define FACE_DETECT_FRAME_HEIGH_REG 	0x0914
#define FACE_DETECT_SMILE_INFO_REG	0x0916
#define FD_SMILE_LEVEL_THRS_REG		0x092a
#define FD_SMILE_DETECT_NUMBER_REG	0x092b
#define FACE_DETECT_DIRECTION_REG 0x094f
enum smile_face_detect {
	SMILE_FACE_DETECT_OFF = 0x00,
	SMILE_FACE_DETECT_ON = 0x01,
};


/*
  * category 0x0A register, AF setting
*/
/*m6mo Category 0xa parameter*/
#define AF_START_REG				0x0a02
enum af_start {
	AF_STOP,
	AF_START,
	AF_RELEASE = 3,
};
#define AF_RESULT_REG				0x0a03
enum af_result {
	AF_OPERATING,
	AF_SUCCESS,
	AF_FAIL,
	AF_STOPPED,
};

#define AF_TOUCH_WIN_W_REG		0x0a22  /* 16 bit */
#define AF_TOUCH_WIN_H_REG		0x0a24
#define AF_TOUCH_WIN_X_REG		0x0a2a
#define AF_TOUCH_WIN_Y_REG		0x0a2c

enum af_touch_info {
	AF_TOUCH_ROW = 0x0a, /*For 10 * 10*/
	AF_TOUCH_COL = 0x0a,
	AF_TOUCH_WIDTH = 0x18,
	AF_TOUCH_HEIGHT = 0x18,
	AF_TOUCH_WIDTH_MAX = 0x100,
	AF_TOUCH_HEIGHT_MAX = 0x100,
};

#define AF_WINDOW_REG				0x0a40
enum af_window {
	CENTRE_SMALL,
	CENTRE_LARGE,
	FIVE_POINT,
	BY_FACE_DETECT,
	BY_USER,
};

#define AF_SCAN_MODE_REG			0x0a41
enum af_scan_mode {
	AF_FULL_SCAN,
	AF_FAST_SCAN = 0x03,
	AF_CONTINUOUS_FOCUS = 0x04,
	AF_FINE_CONTINUOUS_FOCUS = 0x05,
};

/* 
  * category 0xB register, still picture parameter
*/
#define YUVOUT_MAIN_REG			0x0b00  /*capture format, 8 bit*/
enum yuvout_main {
	MAIN_OUTPUT_YUV422,
	MAIN_OUTPUT_JPEG_422,
	MAIN_OUTPUT_JPEG_420,
	MAIN_RAW10_PACK = 5,
	MAIN_RAW8,
	MAIN_RAW10_UNPACK,
	MAIN_RAW10_UNDEFINED8,
};

#define MAIN_IMAGE_SIZE_REG			0x0b01  /*capture image size, 8 bit*/
enum main_image_size {
	MAIN_SIZE_160_120 = 0x00,
	MAIN_SIZE_320_240 = 0x02,
	MAIN_SIZE_426_240 = 0x05,
	MAIN_SIZE_640_480 = 0x09,
	MAIN_SIZE_720p = 0x10,
	MAIN_SIZE_854_480 = 0x0c,
	MAIN_SIZE_1024_768 = 0x0f,
	MAIN_SIZE_1280_960 = 0x14,
	MAIN_SIZE_1600_1200 = 0x17,
	MAIN_SIZE_2048_1536 = 0x1b,	
	MAIN_SIZE_2560_1920 = 0x1f,
	MAIN_SIZE_3264_2448 = 0x25,	
};

#define MAIN_MIRROR_REG		0x0b02
#define MAIN_REVERSE_REG		0x0b03
#define MAIN_ROTATION_REG		0x0b04
#define PREVIEW_ROTATION_REG	0x0b09
#define THUMB_ROTATION_REG	0x0b0e

#define JPEG_RATIO_REG			0x0b17  /* JPEG ratio control, 8 bit */

#define LED_FLASH_CONTROL_REG	 0x0b1f
enum led_flash_control {
	LED_FLASH_OFF,
	LED_FLASH_AUTO = 0x02,
	LED_FLASH_ON = 0x03,
};

#define PART_WDR_EN_REG		0x0b38
enum part_wdr_en {
	PART_WDR_OFF,
	PART_WDR_ON,
	PART_WDR_AUTO,
};

#define PART_WDR_LVL_REG		0x0b39
enum part_wdr_lvl {
	PART_WDR_LOW,
	PART_WDR_MIDDLE,	
	PART_WDR_HIGH,
	PART_WDR_FAIL = 0x0c,
};

#define FLASHLED_SELECT_REG 	0x0b3d    /* m030 or m03x flash led select reg */
enum flashled_select {
	FLASHLED_M030,
	FLASHLED_M03X,   /*default value is m03x*/
};

/*
  * category 0xC register, still picture control
*/
#define CAP_MODE_REG			0x0c00
enum cap_reg_mode {
	CAP_MODE_NORMAL,
	CAP_MODE_AUTO_MULTICAP,
	CAP_MODE_ANTI_HANDSHAKING = 0x0d,
	CAP_MODE_PARORAMA = 0x0e,
};

#define CAP_FRM_COUNT_REG			0x0c02
#define CAP_SEL_FRAME_MAIN_REG	0x0c06  /* main image select, 8 bit */

#define CAP_TRANSFER_START_REG	0x0c09  /* transfer start, 8 bit */
enum cap_transfer_start {
	CAP_TRANSFER_OFF,
	CAP_TRANSFER_MAIN,
	CAP_TRANSFER_PREVIEW,
	CAP_TRANSFER_THUMB,
	CAP_TRANSFER_MAIN1B,
	CAP_TRANSFER_MAIN2B,
	CAP_TRANSFER_MAIN3B,
	CAP_TRANSFER_MAIN4B,
};

#define JPEG_IMAGE_SIZE_REG		0x0c0d  /* main image size, 32 bit */

#define COLOR_BAR_REG 				0x0d1b
enum color_bar {
	ENABLE_COLOR_BAR	= 0x05,
};

/*
  * category 0x0F registers, for flash operation
*/
#define FLASH_ADDRESS_REG 			0x0f00  /* 32 bit */
#define FLASH_SIZE_REG 				0x0f04 /* 16 bit */
#define FLASH_ERASE_CMD_REG 		0x0f06 /* 8 bit */
#define FLASH_WRITE_CMD_REG		0x0f07 /* 8 bit */
#define RAM_CLEAN_CMD_REG 		0x0f08 /* 8 bit */
#define FLASH_CHKSUM_CMD_REG 		0x0f09 /* 8 bit */
#define FLASH_CHKSUM_RESULT_REG 	0x0f0a /* 16 bit */
#define CAMERA_START_CMD_REG 	0x0f12  /* 8 bit */
#define CAMERA_FLASH_TYPE_REG 	0x0f13 /* 8 bit */

#endif
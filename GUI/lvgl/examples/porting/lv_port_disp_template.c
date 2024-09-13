/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "../../lvgl.h"
#include "mylcd.h"
/*********************
 *      DEFINES
 *********************/
#define USE_SRAM 0
#ifndef USE_SRAM
#include "malloc.h"
#endif

#define MY_DISP_HOR_RES (480)
#define MY_DISP_VER_RES (320)


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    
		//第一步初始化设备
		disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
#if USE_SRAM
		//设置缓冲区的大小为屏幕的全尺寸大小
		void *buf_1=mymalloc(SRAMEX,MY_DISP_HOR_RES*MY_DISP_VER_RES);
		//初始化显示缓冲区
		lv_disp_draw_buf_init(&draw_buf_dsc_1,buf_1,NULL,MY_DISP_HOR_RES*MY_DISP_VER_RES);

#else
		//设置缓冲区的大小为10行屏幕大小
		static lv_color_t buf_1[MY_DISP_HOR_RES*10];
		
		//第二步初始化缓冲区
		lv_disp_draw_buf_init(&draw_buf_dsc_1,buf_1,NULL,MY_DISP_HOR_RES*10);

#endif
		
		//第三步  在lvgl中注册显示设备
		static lv_disp_drv_t disp_drv;
		lv_disp_drv_init(&disp_drv);
		
		
		//第四步  设置显示设备的分辨率
		disp_drv.hor_res = lcdDev.width;
		disp_drv.ver_res = lcdDev.height;
		
		
		//第五步  用来将缓冲区的内容复制到显示设备
		disp_drv.flush_cb = disp_flush;
		
		
		//第六步  设置显示缓冲区
		disp_drv.draw_buf = &draw_buf_dsc_1;
		
		
		//第七步  注册显示设备
		lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
		LCD_init();
		
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

		LCD_Fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif

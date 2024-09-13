#include "lv_mainstart.h"
#include "stdio.h"

const lv_font_t *font;

static lv_coord_t scr_act_width, scr_act_height;    /* 活动屏幕宽度、高度 */
static lv_obj_t *parent_obj;                        /* 定义父对象 */
static lv_obj_t *child_obj;                         /* 定义子对象 */

//圆弧
static lv_obj_t *label_arc ;


//进度条
lv_obj_t *label_per = NULL;
lv_obj_t *label_load = NULL;
lv_obj_t *bar = NULL;
uint8_t bar_val = 0;





#if 1

//圆弧
void arc_event_cb(lv_event_t *e){
	lv_obj_t *target = lv_event_get_target(e);
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_VALUE_CHANGED){
		lv_label_set_text_fmt(label_arc,"%d%%",lv_arc_get_value(target));
	}
}
void draw_arc(void){
	lv_obj_t *arc = lv_arc_create(lv_scr_act());
	lv_arc_set_range(arc,0,100);
	lv_arc_set_value(arc,50);
	lv_obj_set_size(arc,200,200);
	lv_obj_align_to(arc,NULL,LV_ALIGN_CENTER,0,0);
	
	//添加事件
	lv_obj_add_event_cb(arc,arc_event_cb,LV_EVENT_VALUE_CHANGED, NULL);
	
	
	
	label_arc = lv_label_create(lv_scr_act());
	lv_obj_align(label_arc,LV_ALIGN_CENTER,0,0);
	lv_label_set_text(label_arc,"0%");
	
	lv_obj_t *label_arc3 = lv_label_create(lv_scr_act());
	lv_obj_align(label_arc3,LV_ALIGN_CENTER,10,10);
	lv_label_set_text(label_arc3,"8920");
	
	
	lv_obj_t *label_arc2 = lv_label_create(lv_scr_act());
	lv_obj_align(label_arc2,LV_ALIGN_CENTER,0,100);
	lv_label_set_text(label_arc2,"123");
	
	lv_task_handler();

}




static void obj_event_cb(lv_event_t *e)
{
    lv_obj_t * target = lv_event_get_target(e);                                 /* 获取事件触发源 */
		printf("pressed\n");
    if (target == parent_obj)                                                   /* 判断触发源：是不是父对象？ */
    {
				printf(" parent_obj pressed\n");
        lv_obj_align(parent_obj, LV_ALIGN_CENTER, 0, 0);                        /* 重新设置父对象的位置：居中 */
    }
    else if (target == child_obj)                                               /* 判断触发源：是不是子对象？ */
    {
				printf(" child_obj pressed\n");
        lv_obj_align(child_obj, LV_ALIGN_RIGHT_MID, 100, 0);                    /* 重新设置子对象的位置：右侧居中，再向X轴偏移100 */
    }
}
//基础部件
void base_test(void){    

    /* 父对象 */
    parent_obj = lv_obj_create(lv_scr_act());                                   /* 创建父对象 */
    lv_obj_set_size(parent_obj, scr_act_width * 2/3, scr_act_height * 2/3);     /* 设置父对象的大小 */
    lv_obj_align(parent_obj, LV_ALIGN_TOP_MID, 0, 0);                           /* 设置父对象的位置：顶部居中 */
    lv_obj_set_style_bg_color(parent_obj, lv_color_hex(0x99ccff), 0);           /* 设置父对象的背景色：浅蓝色 */
    lv_obj_add_event_cb(parent_obj, obj_event_cb, LV_EVENT_LONG_PRESSED, NULL); /* 为父对象添加事件：长按触发 */

    /* 子对象 */
    child_obj = lv_obj_create(parent_obj);                                      /* 创建子对象 */
    lv_obj_set_size(child_obj, scr_act_width / 3, scr_act_height / 3);          /* 设置子对象的大小 */
    lv_obj_align(child_obj, LV_ALIGN_CENTER, 0, 0);                             /* 设置子对象的位置：居中 */
    lv_obj_set_style_bg_color(child_obj, lv_color_hex(0x003366), 0);            /* 设置子对象的背景色：深蓝色 */
    lv_obj_add_event_cb(child_obj, obj_event_cb, LV_EVENT_CLICKED, NULL);       /* 为子对象添加事件：按下释放后触发 */
}

#endif




//画进度条
//定时器回调函数
void timer_cb(lv_timer_t *timer){
	//printf("timer_cb :  %d\n",bar_val);
	if(bar_val < 100){
		bar_val++;
		lv_bar_set_value(bar,bar_val,LV_ANIM_ON);
		lv_label_set_text_fmt(label_per,"%d %%",lv_bar_get_value(bar));
		//lv_label_set_text(label_load,"finished");
	}else{
		//printf("finished\n");
		lv_label_set_text(label_load,"finished");
		const char *updated_text = lv_label_get_text(label_load);
    //printf("Updated label_load text: %s\n", updated_text);
	}
	//lv_timer_handler();
	//lv_tick_inc(5);//这个必须调用，要么不会处理触摸屏事件
}
void LVGL_bar(void){	

	 bar = lv_bar_create(lv_scr_act()); /* 创建进度条 */
	 lv_obj_set_align(bar, LV_ALIGN_CENTER); /* 设置位置 */
	 lv_obj_set_size(bar, scr_act_width * 3 / 5, 20); /* 设置大小 */
	 lv_obj_set_style_anim_time(bar, 100, LV_STATE_DEFAULT); /* 设置动画时间 */
	 lv_timer_create(timer_cb, 100, NULL);
	
		//加载标题标签
		label_load = lv_label_create(lv_scr_act());
		lv_label_set_text(label_load,"LOAD...");
		lv_obj_set_style_text_font(label_load,font,LV_STATE_DEFAULT);
		//lv_obj_align(label_load1,LV_ALIGN_OUT_BOTTOM_MID,0,100);
		lv_obj_align_to(label_load,bar,LV_ALIGN_OUT_BOTTOM_MID,0,10);

	
		label_per = lv_label_create(lv_scr_act());
		lv_label_set_text(label_per,"%0");
		lv_obj_set_style_text_font(label_load,font,LV_STATE_DEFAULT);
		lv_obj_set_align(label_per, LV_ALIGN_CENTER);
}




void lv_mainstart(void)
{
	/* 获取活动屏幕的高度 */
	/* 动态获取屏幕大小 */
	scr_act_width = lv_obj_get_width(lv_scr_act());                             /* 获取活动屏幕的宽度 */
	scr_act_height = lv_obj_get_height(lv_scr_act());
	printf("%d %d \n",scr_act_width,scr_act_height);
	//设置字体
	font = &lv_font_montserrat_14;
	
	
	LVGL_bar();
}





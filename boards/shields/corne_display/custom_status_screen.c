#include "custom_status_screen.h"
#include "widgets/layer_status.h"
#include "widgets/bongo_cat.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_layer_status layer_status_widget;
static struct zmk_widget_bongo_cat bongo_cat_widget;

lv_style_t global_style;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;

lv_disp_drv_t disp_drv;
lv_disp_drv_init(&disp_drv);
disp_drv.buffer = &disp_buf;
disp_drv.flush_cb = lvgl_flush_cb;
disp_drv.sw_rotate = 1;   // add for rotation
disp_drv.rotated = LV_DISP_ROT_90;   // add for rotation
lv_disp_drv_register(&disp_drv);

    screen = lv_obj_create(NULL);

    zmk_widget_bongo_cat_init(&bongo_cat_widget, screen);
	zmk_widget_layer_status_init(&layer_status_widget, screen);
	lv_obj_align(zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_CENTER, 30, 0);

    return screen;
}

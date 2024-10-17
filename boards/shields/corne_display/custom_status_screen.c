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

    screen = lv_obj_create(NULL);

    zmk_widget_bongo_cat_init(&bongo_cat_widget, screen);
	zmk_widget_layer_status_init(&layer_status_widget, screen);
	lv_obj_align(zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_TOP_RIGHT, -10, 0);

    return screen;
}

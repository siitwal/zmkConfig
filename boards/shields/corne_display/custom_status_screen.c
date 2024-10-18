#include "custom_status_screen.h"
#include "widgets/label_layer_status.h"
#include "widgets/kb_status.h"
#include "widgets/bongo_cat.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_label_layer_status widget_label_layer_status;
static struct zmk_widget_kb_status widget_kb_status;
static struct zmk_widget_bongo_cat widget_bongo_cat;

lv_style_t global_style;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;

    screen = lv_obj_create(NULL);

    zmk_widget_bongo_cat_init(&widget_bongo_cat, screen);
	zmk_widget_label_layer_status_init(&widget_label_layer_status, screen);
	zmk_widget_kb_status_init(&widget_kb_status, screen);
	lv_obj_align(zmk_widget_bongo_cat_obj(&widget_bongo_cat), LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_align(zmk_widget_label_layer_status_obj(&widget_label_layer_status), LV_ALIGN_CENTER, 40, 0);
	lv_obj_align(zmk_widget_kb_status_obj(&widget_kb_status), LV_ALIGN_CENTER, 20, 0);

    return screen;
}

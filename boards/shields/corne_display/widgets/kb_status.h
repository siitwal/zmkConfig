#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define LAYER_CANVAS_WIDTH 32
#define LAYER_CANVAS_HEIGHT 32

struct zmk_widget_kb_status {
    sys_snode_t node;
    lv_obj_t *obj;
	lv_color_t battery_cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT)];
};

int zmk_widget_kb_status_init(struct zmk_widget_kb_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_kb_status_obj(struct zmk_widget_kb_status *widget);

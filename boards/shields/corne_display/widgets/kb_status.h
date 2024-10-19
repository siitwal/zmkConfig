#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zmk/endpoints.h>

#define LAYER_CANVAS_WIDTH 40
#define LAYER_CANVAS_HEIGHT 40

struct status_state {
    uint8_t battery;
    bool charging;
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
    uint8_t layer_index;
    const char *layer_label;
};

struct zmk_widget_kb_status {
    sys_snode_t node;
    lv_obj_t *obj;
	lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT)];
	struct status_state state;
};

int zmk_widget_kb_status_init(struct zmk_widget_kb_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_kb_status_obj(struct zmk_widget_kb_status *widget);

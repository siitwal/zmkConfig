#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_image {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_image_init(struct zmk_widget_image *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_image_obj(struct zmk_widget_image *widget);

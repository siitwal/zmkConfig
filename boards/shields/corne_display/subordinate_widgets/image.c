#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/display.h>

#include "image.h"


LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);
LV_IMG_DECLARE(main_image);


static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);


int zmk_widget_image_init(struct zmk_widget_image *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent);

	lv_img_set_src(widget->obj, &main_image);
    lv_obj_align(widget->obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(widget->obj, 83, 32);

    sys_slist_append(&widgets, &widget->node);


    return 0;
}

lv_obj_t *zmk_widget_image_obj(struct zmk_widget_image *widget) { return widget->obj; }

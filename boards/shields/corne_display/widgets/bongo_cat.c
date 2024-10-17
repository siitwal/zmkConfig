#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include "bongo_cat.h"

#define SRC(array) (const void **)array, sizeof(array) / sizeof(lv_img_dsc_t *)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

LV_IMG_DECLARE(bongo_cat_idle0);
LV_IMG_DECLARE(bongo_cat_idle1);
LV_IMG_DECLARE(bongo_cat_idle2);
LV_IMG_DECLARE(bongo_cat_idle3);
LV_IMG_DECLARE(bongo_cat_idle4);
LV_IMG_DECLARE(bongo_cat_right_tap);
LV_IMG_DECLARE(bongo_cat_left_tap);
LV_IMG_DECLARE(bongo_cat_none_tap);
LV_IMG_DECLARE(bongo_cat_both_tap);

#define ANIMATION_SPEED_IDLE 2000
const lv_img_dsc_t *idle_imgs[] = {
    &bongo_cat_idle0,
    &bongo_cat_idle1,
    &bongo_cat_idle2,
    &bongo_cat_idle3,
    &bongo_cat_idle4,
};

#define ANIMATION_SPEED_SLOW 2400
const lv_img_dsc_t *slow_imgs[] = {
    &bongo_cat_none_tap,
    &bongo_cat_left_tap,
    &bongo_cat_both_tap,
    &bongo_cat_right_tap,
};

#define ANIMATION_SPEED_MID 1200
const lv_img_dsc_t *mid_imgs[] = {
    &bongo_cat_none_tap,
    &bongo_cat_left_tap,
    &bongo_cat_both_tap,
    &bongo_cat_right_tap,
};

#define ANIMATION_SPEED_FAST 250
const lv_img_dsc_t *fast_imgs[] = {
    &bongo_cat_left_tap,
    &bongo_cat_right_tap,
};

struct bongo_cat_wpm_status_state {
    uint8_t wpm;
};

enum anim_state {
    anim_state_none,
    anim_state_idle,
    anim_state_slow,
    anim_state_mid,
    anim_state_fast
} current_anim_state;


struct bongo_cat_wpm_status_state bongo_cat_wpm_status_get_state(const zmk_event_t *eh) {
    struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(eh);
    return (struct bongo_cat_wpm_status_state) { .wpm = ev->state };
};

void bongo_cat_wpm_status_update_cb(struct bongo_cat_wpm_status_state state) {
    struct zmk_widget_bongo_cat *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_animation(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_bongo_cat, struct bongo_cat_wpm_status_state,
                            bongo_cat_wpm_status_update_cb, bongo_cat_wpm_status_get_state)

ZMK_SUBSCRIPTION(widget_bongo_cat, zmk_wpm_state_changed);

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {
    widget->obj = lv_animimg_create(parent);
    lv_obj_center(widget->obj);

    sys_slist_append(&widgets, &widget->node);

    widget_bongo_cat_init();

    return 0;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) {
    return widget->obj;
}

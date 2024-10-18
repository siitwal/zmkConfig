#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/usb.h>

#include "kb_status.h"


LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);


static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_state {
    uint8_t level;
    bool usb_present;
};


static void draw_kb_status(lv_obj_t *widget, lv_color_t cbuf[], const struct battery_state state) {
	lv_obj_t *canvas = lv_obj_get_child(widget, 0);
	
	char text[9] = {};
    uint8_t level = state.level;

    if (state.usb_present) strcpy(text, LV_SYMBOL_CHARGE " ");

    char perc[6] = {};
    snprintf(perc, sizeof(perc), "%3u%%", level);
    strcat(text, perc);

    // if (level > 95) strcat(text, LV_SYMBOL_BATTERY_FULL);
    // else if (level > 65) strcat(text, LV_SYMBOL_BATTERY_3);
    // else if (level > 35) strcat(text, LV_SYMBOL_BATTERY_2);
    // else if (level > 5) strcat(text, LV_SYMBOL_BATTERY_1);
    // else strcat(text, LV_SYMBOL_BATTERY_EMPTY);

	lv_draw_rect_dsc_t rect_black_dsc;
	lv_draw_rect_dsc_init(&rect_black_dsc);
    rect_black_dsc.bg_color = lv_color_white();
	lv_canvas_draw_rect(canvas, 0, 0, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT, &rect_black_dsc);


	lv_draw_label_dsc_t label;
	lv_draw_label_dsc_init(&label);
    label.color = lv_color_black();
    label.font = &lv_font_montserrat_8;
    label.align = LV_TEXT_ALIGN_LEFT;
	lv_canvas_draw_text(canvas, 0, 0, LAYER_CANVAS_WIDTH, &label, text);


	static lv_color_t cbuf_tmp[LAYER_CANVAS_WIDTH * LAYER_CANVAS_HEIGHT];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = LAYER_CANVAS_WIDTH;
    img.header.h = LAYER_CANVAS_HEIGHT;
    lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);
    lv_canvas_transform(canvas, &img, 900, LV_IMG_ZOOM_NONE, 0, 0, LAYER_CANVAS_WIDTH / 2, LAYER_CANVAS_HEIGHT / 2, true);

}

void battery_update(struct battery_state state) {
    struct zmk_widget_kb_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { draw_kb_status(widget->obj, widget->cbuf, state); }
}

static struct battery_state battery_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
        .usb_present = zmk_usb_is_powered(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery, struct battery_state, battery_update, battery_get_state)

ZMK_SUBSCRIPTION(widget_battery, zmk_battery_state_changed);
ZMK_SUBSCRIPTION(widget_battery, zmk_usb_conn_state_changed);


int zmk_widget_kb_status_init(struct zmk_widget_kb_status *widget, lv_obj_t *parent) {
	widget->obj = lv_obj_create(parent);
	lv_obj_set_size(widget->obj, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT);
	
	lv_obj_t *kb = lv_canvas_create(widget->obj);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_canvas_set_buffer(kb, widget->cbuf, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    sys_slist_append(&widgets, &widget->node);
    widget_battery_init();

    return 0;
}

lv_obj_t *zmk_widget_kb_status_obj(struct zmk_widget_kb_status *widget) { return widget->obj; }

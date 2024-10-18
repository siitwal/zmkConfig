#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/display.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

#include "label_layer_status.h"


LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);


static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct label_layer_status_state {
    uint8_t index;
    const char *label;
};


void init_rect(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color) {
    lv_draw_rect_dsc_init(rect_dsc);
    rect_dsc->bg_color = bg_color;
}

void init_label(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font, lv_text_align_t align) {
    lv_draw_label_dsc_init(label_dsc);
    label_dsc->color = color;
    label_dsc->font = font;
    label_dsc->align = align;
}

static void draw_layer(lv_obj_t *widget, lv_color_t cbuf[], const struct label_layer_status_state state) {
	lv_obj_t *canvas = lv_obj_get_child(widget, 0);

	lv_draw_rect_dsc_t rect_black_dsc;
	init_rect(&rect_black_dsc, lv_color_white());
	lv_canvas_draw_rect(canvas, 0, 0, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT, &rect_black_dsc);


	lv_draw_label_dsc_t label;
    init_label(&label, lv_color_black(), &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT);
	
	char text[4] = {};
	snprintf(text, sizeof(text), "%s", state.label);

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

static void label_layer_status_update(struct label_layer_status_state state) {
    struct zmk_widget_label_layer_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { draw_layer(widget->obj, widget->cbuf, state); }
}

static struct label_layer_status_state label_layer_status_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct label_layer_status_state) { .index = index, .label = zmk_keymap_layer_name(index) };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_label_layer_status, struct label_layer_status_state, 
		label_layer_status_update, label_layer_status_get_state)

ZMK_SUBSCRIPTION(widget_label_layer_status, zmk_layer_state_changed);

int zmk_widget_label_layer_status_init(struct zmk_widget_label_layer_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
	lv_obj_set_size(widget->obj, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT);
	
	lv_obj_t *layer = lv_canvas_create(widget->obj);
    lv_obj_align(layer, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_buffer(layer, widget->cbuf, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    sys_slist_append(&widgets, &widget->node);
    widget_label_layer_status_init();

    return 0;
}

lv_obj_t *zmk_widget_label_layer_status_obj(struct zmk_widget_label_layer_status *widget) {
    return widget->obj;
}

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/battery.h>
#include <zmk/usb.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/layer_state_changed.h>

#include "kb_status.h"


LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);


static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_state {
    uint8_t level;
    bool charging;
};

struct output_state {
    struct zmk_endpoint_instance selected_endpoint;
    bool connected;
    bool bonded;
};

struct layer_state {
    uint8_t index;
    const char *label;
};



static void draw_kb_status(struct zmk_widget_kb_status *widget) {
	lv_obj_t *canvas = lv_obj_get_child(widget-> obj, 0);
	lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);
	
	char text[9] = {};
	
	if (widget->state->layer_index == 0) {
		snprintf(text, sizeof(text), "%3u", widget->state->battery);
		if (widget->state->charging) strcat(text, LV_SYMBOL_CHARGE);
		else strcat(text, "%");
	} else {
		switch (widget->state->selected_endpoint.transport) {
		case ZMK_TRANSPORT_USB:
			strcat(text, LV_SYMBOL_USB);
			break;
		case ZMK_TRANSPORT_BLE:
			if (widget->state->active_profile_bonded) {
				if (widget->state->active_profile_connected) {
					snprintf(text, sizeof(text), LV_SYMBOL_WIFI " %i " LV_SYMBOL_OK, 
							widget->state->selected_endpoint.ble.profile_index + 1);
            }	else {
					snprintf(text, sizeof(text), LV_SYMBOL_WIFI " %i " LV_SYMBOL_CLOSE,
							widget->state->selected_endpoint.ble.profile_index + 1);
				}
			} else {
				snprintf(text, sizeof(text), LV_SYMBOL_WIFI " %i " LV_SYMBOL_SETTINGS,
							widget->state->selected_endpoint.ble.profile_index + 1);
			}
			break;
		}
	}

	lv_draw_label_dsc_t label;
	lv_draw_label_dsc_init(&label);
    label.color = lv_color_black();
    label.font = &lv_font_montserrat_12;
    label.align = LV_TEXT_ALIGN_CENTER;
	lv_canvas_draw_text(canvas, 0, 6, LAYER_CANVAS_WIDTH, &label, text);

	lv_draw_rect_dsc_t rect_black_dsc;
	lv_draw_rect_dsc_init(&rect_black_dsc);
    rect_black_dsc.bg_color = lv_color_black();
	lv_canvas_draw_rect(canvas, 0, 0, LAYER_CANVAS_WIDTH, 2, &rect_black_dsc);

	static lv_color_t cbuf_tmp[LAYER_CANVAS_WIDTH * LAYER_CANVAS_HEIGHT];
    memcpy(cbuf_tmp, widget->cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = LAYER_CANVAS_WIDTH;
    img.header.h = LAYER_CANVAS_HEIGHT;
    lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);
    lv_canvas_transform(canvas, &img, 900, LV_IMG_ZOOM_NONE, 0, 0, LAYER_CANVAS_WIDTH / 2, LAYER_CANVAS_HEIGHT / 2, true);
}

// BEGIN LAYER

void set_layer_state(struct zmk_widget_kb_status *widget, struct layer_state *state) {
	widget->state->layer_index = state->index;
	widget->state->layer_label = state->label;
}

static void layer_update(struct layer_state state) {
    struct zmk_widget_kb_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		set_layer_state(widget, &state);
		draw_kb_status(widget);
	}
}

static struct layer_state layer_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_state) { .index = index, .label = zmk_keymap_layer_name(index) };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer, struct layer_state, layer_update, layer_get_state)
ZMK_SUBSCRIPTION(widget_layer, zmk_layer_state_changed);
//END LAYER


// BEGIN OUTPUT
void set_output_state(struct zmk_widget_kb_status *widget, struct output_state *state) {
	widget->state->selected_endpoint = state->selected_endpoint;
	widget->state->active_profile_connected = state->connected;
	widget->state->active_profile_bonded = state->bonded;
}

static struct output_state output_get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
		.selected_endpoint = zmk_endpoints_selected(),
        .connected = zmk_ble_active_profile_is_connected(),
        .bonded = !zmk_ble_active_profile_is_open()
	};
}

static void output_update(struct output_state state) {
    struct zmk_widget_kb_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		set_output_state(widget, &state);
		draw_kb_status(widget);
	}
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output, struct output_state, output_update, output_get_state)
ZMK_SUBSCRIPTION(widget_output, zmk_endpoint_changed);
ZMK_SUBSCRIPTION(widget_output, zmk_ble_active_profile_changed);
// END OUTPUT


// BEGIN BATTERY
void set_battery_state(struct zmk_widget_kb_status *widget, struct battery_state *state) {
	widget->state->battery = state->level;
	widget->state->charging = state->usb_present;
}

void battery_update(struct battery_state state) {
    struct zmk_widget_kb_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
		set_battery_state(widget, &state);
		draw_kb_status(widget);
	}
}

static struct battery_state battery_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
        .charging = zmk_usb_is_powered(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery, struct battery_state, battery_update, battery_get_state)
ZMK_SUBSCRIPTION(widget_battery, zmk_battery_state_changed);
ZMK_SUBSCRIPTION(widget_battery, zmk_usb_conn_state_changed);
// END BATTERY

int zmk_widget_kb_status_init(struct zmk_widget_kb_status *widget, lv_obj_t *parent) {
	widget->obj = lv_obj_create(parent);
	lv_obj_set_size(widget->obj, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT);
	
	lv_obj_t *kb = lv_canvas_create(widget->obj);
    lv_obj_align(kb, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(kb, widget->cbuf, LAYER_CANVAS_WIDTH, LAYER_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    sys_slist_append(&widgets, &widget->node);

    widget_layer_init();
    widget_output_init();
    widget_battery_init();

    return 0;
}

lv_obj_t *zmk_widget_kb_status_obj(struct zmk_widget_kb_status *widget) { return widget->obj; }

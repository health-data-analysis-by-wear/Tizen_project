#include "hda_service.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;

	Evas_Object *box_main;

	Evas_Object *btn_slight;
	Evas_Object *btn_sharp;
	Evas_Object *btn_slight_text;
	Evas_Object *btn_sharp_text;

	Evas_Object *bg;

	Evas_Object *nv;
	Elm_Object_Item *nv_it;
} appdata_s;

static void clicked_slight(void *user_data, Evas* e,  Evas_Object *obj, void *event_info);
static void clicked_sharp(void *user_data, Evas* e,  Evas_Object *obj, void *event_info);

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);


	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);


	ad->bg = elm_bg_add(ad->conform);
	elm_bg_color_set(ad->bg, 255, 255, 255);
	elm_object_content_set(ad->conform, ad->bg);

	ad->box_main = elm_box_add(ad->bg);
	elm_box_horizontal_set(ad->box_main, EINA_TRUE);
	elm_box_padding_set(ad->box_main, 0, 0);
	elm_object_content_set(ad->bg, ad->box_main);
	evas_object_show(ad->box_main);


	// slight button
	ad->btn_slight = elm_button_add(ad->box_main);
	evas_object_color_set(ad->btn_slight, 0, 235, 100, 255);
	elm_object_style_set(ad->btn_slight, "default");
	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
	evas_object_size_hint_weight_set(ad->btn_slight, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->btn_slight, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(ad->btn_slight, "<b>SLIGHT</b>");
	evas_object_show(ad->btn_slight);
	elm_box_pack_start(ad->box_main, ad->btn_slight);

	// slight button event
	evas_object_smart_callback_add(ad->btn_slight, "clicked", clicked_slight, ad);


	// sharp button
	ad->btn_sharp = elm_button_add(ad->box_main);
	evas_object_color_set(ad->btn_sharp, 254, 71, 0, 255);
	elm_object_style_set(ad->btn_slight, "default");
	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
	evas_object_size_hint_weight_set(ad->btn_sharp, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->btn_sharp, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(ad->btn_sharp, "<b>SHARP</b>");
	evas_object_show(ad->btn_sharp);
	elm_box_pack_end(ad->box_main, ad->btn_sharp);

	// sharp button event
	evas_object_smart_callback_add(ad->btn_sharp, "clicked", clicked_sharp, ad);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}

//////////////////////////////////////////////////////////////////
///////////////////////////// Screen /////////////////////////////
//////////////////////////////////////////////////////////////////

static void activated_screen(appdata_s *ad){
	ad->bg = elm_bg_add(ad->nv);
	elm_bg_color_set(ad->bg, 255, 255, 255);
	elm_object_content_set(ad->conform, ad->bg);

	ad->box_main = elm_box_add(ad->bg);
	elm_box_horizontal_set(ad->box_main, EINA_TRUE);
	elm_box_padding_set(ad->box_main, 0, 0);
	elm_object_content_set(ad->bg, ad->box_main);
	evas_object_show(ad->box_main);

	ad->nv_it = elm_naviframe_item_push(ad->nv, NULL, NULL, NULL, ad->bg, NULL);
	elm_naviframe_item_title_enabled_set(ad->nv_it, EINA_FALSE, EINA_FALSE);

	// slight button
	ad->btn_slight = elm_button_add(ad->box_main);
	evas_object_color_set(ad->btn_slight, 0, 235, 100, 255);
	elm_object_style_set(ad->btn_slight, "default");
	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
	evas_object_size_hint_weight_set(ad->btn_slight, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->btn_slight, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(ad->btn_slight, "<b>SLIGHT</b>");
	evas_object_show(ad->btn_slight);
	elm_box_pack_start(ad->box_main, ad->btn_slight);

	// slight button event
	evas_object_smart_callback_add(ad->btn_slight, "clicked", clicked_slight, ad);

	// sharp button
	ad->btn_sharp = elm_button_add(ad->box_main);
	evas_object_color_set(ad->btn_sharp, 254, 71, 0, 255);
	elm_object_style_set(ad->btn_slight, "default");
	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
	evas_object_size_hint_weight_set(ad->btn_sharp, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->btn_sharp, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_text_set(ad->btn_sharp, "<b>SHARP</b>");
	evas_object_show(ad->btn_sharp);
	elm_box_pack_end(ad->box_main, ad->btn_sharp);

	// sharp button event
	evas_object_smart_callback_add(ad->btn_sharp, "clicked", clicked_sharp, ad);

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
//	ad->label = elm_label_add(ad->conform);
//	elm_object_text_set(ad->label, "<align=center>Hello Tizen</align>");
//	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//	elm_object_content_set(ad->conform, ad->label);
}


/////////////////////////////////////////////////////////////////
///////////////////////////// Event /////////////////////////////
/////////////////////////////////////////////////////////////////

static void clicked_slight(void *user_data, Evas* e,  Evas_Object *obj, void *event_info){
	 dlog_print(DLOG_INFO, "HDA_EVENT", "Slight");

	 Evas_Object *popup;
	 appdata_s *ad = user_data;
	 popup = elm_popup_add(ad->win);
	 elm_object_style_set(popup, "toast");
	 evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	 eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
	 elm_object_text_set(popup, "Clicked slight");
	 elm_popup_timeout_set(popup, 2.0);
	 evas_object_show(popup);
}
static void clicked_sharp(void *user_data, Evas* e,  Evas_Object *obj, void *event_info){
	 dlog_print(DLOG_INFO, "HDA_EVENT", "Sharp");

	 Evas_Object *popup;
	 appdata_s *ad = user_data;
	 popup = elm_popup_add(ad->win);
	 elm_object_style_set(popup, "toast");
	 evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	 eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
	 elm_object_text_set(popup, "Clicked sharp");
	 elm_popup_timeout_set(popup, 3.0);
	 evas_object_show(popup);
}




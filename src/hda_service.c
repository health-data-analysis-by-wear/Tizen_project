#include "hda_service.h"
#include <privacy_privilege_manager.h>

#include <sensor/hrm_listener.h>
#include <sensor/physics_listener.h>

#include "bluetooth/gatt/server.h"
#include "bluetooth/gatt/service.h"
#include "bluetooth/gatt/characteristic.h"
#include "bluetooth/gatt/descriptor.h"
#include "bluetooth/le/advertiser.h"


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

sensor_type_e sensor_type = SENSOR_HRM;
sensor_h sensor_handle = 0;

static void clicked_slight(void *user_data, Evas* e,  Evas_Object *obj, void *event_info);
static void clicked_sharp(void *user_data, Evas* e,  Evas_Object *obj, void *event_info);


bool check_hrm_sensor_is_supported();
bool initialize_hrm_sensor();

const char *sensor_privilege = "http://tizen.org/privilege/healthinfo";

bool check_and_request_sensor_permission();
bool request_sensor_permission();
void request_sensor_permission_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data);

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
	int retval;
	bt_adapter_state_e bluetooth_adapter_state;

	appdata_s *ad = data;

	create_base_gui(ad);

	if(!check_hrm_sensor_is_supported())
	{
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "A HRM sensor is not supported.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "A HRM sensor is supported.");
	retval = bt_initialize();
	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "Function bt_initialize() return value = %s", get_error_message(retval));
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to initialize the Bluetooth API.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in initializing the Bluetooth API.");
	retval = bt_adapter_get_state(&bluetooth_adapter_state);
	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "Function bt_adapter_get_state() return value = %s", get_error_message(retval));
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to get the current state of local Bluetooth adapter.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in getting the current state of local Bluetooth adapter.");
	if (bluetooth_adapter_state == BT_ADAPTER_DISABLED)
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Bluetooth adapter is disabled.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Bluetooth adapter is enabled.");
	retval = bt_gatt_server_initialize();
	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "Function bt_gatt_server_initialize() return value = %s", get_error_message(retval));
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to initialize the GATT server.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in initializing the GATT server.");
	if(!create_gatt_descriptor())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to create the GATT characteristic descriptor.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in creating the GATT characteristic descriptor.");
	if(!create_gatt_characteristic())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to create the GATT characteristic.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in creating the GATT characteristic.");
	if(!create_gatt_service())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to create the GATT service.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in creating the GATT service.");
	if(!create_gatt_server())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to create the GATT server's handle.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in creating the GATT server's handle.");
	if(!start_gatt_server())
	{
	dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to register the application along with the GATT services of the application it is hosting.");
			return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in registering the application along with the GATT services of the application it is hosting.");
	if(!create_bluetooth_le_advertiser())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to create advertiser to advertise device's existence.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in creating advertiser to advertise device's existence.");
	if(!start_bluetooth_le_advertising())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to start advertising with passed advertiser and advertising parameters.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in starting advertising with passed advertiser and advertising parameters.");

//	create_physics_sensor_listener();
//	sensor_launch_all();





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
	if (!check_and_request_sensor_permission())
	{
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to check if an application has permission to use the sensor privilege.");
		ui_app_exit();
	}
	else
		dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in checking if an application has permission to use the sensor privilege.");

}

static void
app_terminate(void *data)
{
	/* Release all resources. */
	int retval;

	if(check_hrm_sensor_listener_is_created())
	{
		if(!destroy_hrm_sensor_listener())
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to release all the resources allocated for a HRM sensor listener.");
		else
			dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in releasing all the resources allocated for a HRM sensor listener.");
	}
	if(!destroy_gatt_service())
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to destroy the GATT handle of service.");
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in destroying the GATT handle of service.");
	if(!destroy_gatt_server())
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to destroy the GATT server's handle.");
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in destroying the GATT server's handle.");

	retval = bt_deinitialize();
	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "Function bt_deinitialize() return value = %s", get_error_message(retval));
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Failed to release all resources of the Bluetooth API.");
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded in releasing all resources of the Bluetooth API.");
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

static void clicked_slight(void *user_data, Evas* e,  Evas_Object *obj, void *event_info)
{
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
static void clicked_sharp(void *user_data, Evas* e,  Evas_Object *obj, void *event_info)
{
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

bool check_hrm_sensor_is_supported()
{
	int retval;
	bool supported = false;

	retval = sensor_is_supported(sensor_type, &supported);

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function sensor_is_supported() return value = %s", get_error_message(retval));
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to checks whether a HRM sensor is supported in the current device.");
		return false;
	}
	else
		dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in checking whether a HRM sensor is supported in the current device.");

	if(!supported)
	{
		dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function sensor_is_supported() output supported = %d", supported);
		return false;
	}
	else
		return true;
}

bool initialize_hrm_sensor()
{
	int retval;

	retval = sensor_get_default_sensor(sensor_type, &sensor_handle);

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function sensor_get_default_sensor() return value = %s", get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool check_and_request_sensor_permission()
{
	int retval;
	ppm_check_result_e result;

	retval = ppm_check_permission(sensor_privilege, &result);

	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
	{
		switch (result)
		{
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, LOG_TAG, "The application has permission to use a sensor privilege.");

			if(!check_hrm_sensor_listener_is_created())
			{
				if(!initialize_hrm_sensor())
				{
					dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to get the handle for the default sensor of a HRM sensor.");
					return false;
				}
				else
					dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in getting the handle for the default sensor of a HRM sensor.");

				if(!create_hrm_sensor_listener(sensor_handle))
				{
					dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to create a HRM sensor listener.");
					return false;
				}
				else
					dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in creating a HRM sensor listener.");

				if(!start_hrm_sensor_listener())
					dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to start observing the sensor events regarding a HRM sensor listener.");
				else
					dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in starting observing the sensor events regarding a HRM sensor listener.");
			}
			return true;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY");
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "The application doesn't have permission to use a sensor privilege.");
			return false;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
			dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "The user has to be asked whether to grant permission to use a sensor privilege.");

			if(!request_sensor_permission())
			{
				dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to request a user's response to obtain permission for using the sensor privilege.");
				return false;
			}
			else
			{
				dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in requesting a user's response to obtain permission for using the sensor privilege.");
				return true;
			}
		}
	}
	else
	{
		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
		/* Handle errors */
		dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function ppm_check_permission() return %s", get_error_message(retval));
		return false;
	}
}

bool request_sensor_permission()
{
	int retval;

	retval = ppm_request_permission(sensor_privilege, request_sensor_permission_response_callback, NULL);

	/* Log and handle errors */
	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
		return true;
	else if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_ALREADY_IN_PROGRESS)
		return true;
	else
	{
		dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function ppm_request_permission() return value = %s", get_error_message(retval));
		return false;
	}
}

void request_sensor_permission_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data)
{
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR)
	{
		/* Log and handle errors */
		dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function request_sensor_permission_response_callback() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR");
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Function request_sensor_permission_response_callback() was called because of an error.");
	}
	else
	{
		dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Function request_sensor_permission_response_callback() was called with a valid answer.");

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "The user granted permission to use a sensor privilege for an indefinite period of time.");

			if(!initialize_hrm_sensor())
			{
				dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to get the handle for the default sensor of a HRM sensor.");
				ui_app_exit();
			}
			else
				dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in getting the handle for the default sensor of a HRM sensor.");

			if(!create_hrm_sensor_listener(sensor_handle))
			{
				dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to create a HRM sensor listener.");
				ui_app_exit();
			}
			else
				dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in creating a HRM sensor listener.");

			if(!start_hrm_sensor_listener())
			{
				dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Failed to start observing the sensor events regarding a HRM sensor listener.");
				ui_app_exit();
			}
			else
				dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Succeeded in starting observing the sensor events regarding a HRM sensor listener.");
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER");
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "The user denied granting permission to use a sensor privilege for an indefinite period of time.");
			ui_app_exit();
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
			/* Show a message with explanation */
			dlog_print(DLOG_DEBUG, SENSOR_LOG_TAG, "Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE");
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "The user denied granting permission to use a sensor privilege once.");
			ui_app_exit();
			break;
		}
	}
}



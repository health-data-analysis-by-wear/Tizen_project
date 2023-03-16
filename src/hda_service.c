#include "hda_service.h"
#include <privacy_privilege_manager.h>

#include <sensor/hrm_listener.h>
#include <sensor/physics_listener.h>
#include <sensor/environment_listener.h>
#include <tools/sqlite_helper.h>
#include "bluetooth/gatt/server.h"
#include "bluetooth/gatt/service.h"
#include "bluetooth/gatt/characteristic.h"
#include "bluetooth/gatt/descriptor.h"
#include "bluetooth/le/advertiser.h"

#include <app_common.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;

//	Evas_Object *box_main;

//	Evas_Object *btn_slight;
//	Evas_Object *btn_sharp;
//	Evas_Object *btn_slight_text;
//	Evas_Object *btn_sharp_text;

	Evas_Object *grid_main;

	Evas_Object *btn_level_0;
	Evas_Object *btn_level_1;
	Evas_Object *btn_level_2;
	Evas_Object *btn_level_3;
	Evas_Object *btn_level_0_text;
	Evas_Object *btn_level_1_text;
	Evas_Object *btn_level_2_text;
	Evas_Object *btn_level_3_text;

	Evas_Object *bg;

	Evas_Object *nv;
	Elm_Object_Item *nv_it;
} appdata_s;

sensor_type_e sensor_type = SENSOR_HRM;

sensor_h hrm_sensor_handle = 0;
sensor_h hrm_led_green_sensor_handle = 0;

sensor_h accelerometer_sensor_handle = 0;
sensor_h gravity_sensor_handle = 0;
sensor_h gyroscope_rotation_vector_sensor_hanlde = 0;
sensor_h gyroscope_sensor_handle = 0;
sensor_h linear_acceleration_sensor_handle = 0;

sensor_h light_sensor_handle = 0;
sensor_h pedometer_handle = 0;
sensor_h pressure_sensor_handle = 0;
sensor_h sleep_monitor_handle = 0;

sqlite3 *sql_db;

static void clicked_slight(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);
static void clicked_sharp(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info);

static Eina_Bool _do_animation(void *data, double pos);

bool check_hrm_sensor_is_supported();
bool initialize_hrm_sensor();
bool initialize_hrm_led_green_sensor();

bool check_physics_sensor_is_supported();
bool initialize_accelerometer_sensor();
bool initialize_gravity_sensor();
bool initialize_gyroscope_rotation_vector_sensor();
bool initialize_gyroscope_sensor();
bool initialize_linear_acceleration_sensor();

bool check_environment_sensor_is_supported();
bool initialize_light_sensor();
bool initialize_pedometer();
bool initialize_pressure_sensor();
bool initialize_sleep_monitor();

const char *sensor_privilege = "http://tizen.org/privilege/healthinfo";
const char *mediastorage_privilege = "http://tizen.org/privilege/mediastorage";

bool check_and_request_sensor_permission();
bool request_sensor_permission();
//void request_sensor_permission_response_callback(ppm_call_cause_e cause,
//		ppm_request_result_e result, const char *privilege, void *user_data);
void request_health_sensor_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data);
void request_physics_sensor_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data);

static void win_delete_request_cb(void *data, Evas_Object *obj,
		void *event_info) {
	ui_app_exit();
}

static void win_back_cb(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void create_base_gui(appdata_s *ad) {
	/* Window */
	/* Create and initialize elm_win.
	 elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win,
				(const int *) (&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request",
			win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb,
			ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	 elm_conformant is mandatory for base gui to have proper size
	 when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->bg = elm_bg_add(ad->conform);
	elm_bg_color_set(ad->bg, 255, 255, 255);
	elm_object_content_set(ad->conform, ad->bg);

//	ad->box_main = elm_box_add(ad->bg);
//	elm_box_horizontal_set(ad->box_main, EINA_TRUE);
//	elm_box_padding_set(ad->box_main, 0, 0);
//	elm_object_content_set(ad->bg, ad->box_main);
//	evas_object_show(ad->box_main);


	ad->grid_main = elm_grid_add(ad->conform);
	elm_object_content_set(ad->conform, ad->grid_main);
	evas_object_show(ad->grid_main);

	ad->btn_level_0 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_0, 0, 255, 91, 255);
	elm_grid_pack(ad->grid_main, ad->btn_level_0, 0, 0, 50, 50);
	evas_object_show(ad->btn_level_0);
	ad->btn_level_0_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_0_text, 255, 255, 255, 255);
	elm_object_text_set(ad->btn_level_0_text, "<align=center><font_size=30><b>통증 없음</b></font></align>");
	elm_grid_pack(ad->grid_main, ad->btn_level_0_text, 0, 25, 50, 10);
	evas_object_show(ad->btn_level_0_text);
	evas_object_event_callback_add(ad->btn_level_0, EVAS_CALLBACK_MOUSE_DOWN, clicked_slight, ad);

	ad->btn_level_1 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_1, 255, 245, 85, 255);
	elm_grid_pack(ad->grid_main, ad->btn_level_1, 50, 0, 50, 50);
	evas_object_show(ad->btn_level_1);
	ad->btn_level_1_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_1_text, 255, 255, 255, 255);
	elm_object_text_set(ad->btn_level_1_text,
			"<align=center><font_size=30><b>약한 통증</b></font></align>");
	elm_grid_pack(ad->grid_main, ad->btn_level_1_text, 50, 25, 50, 10);
	evas_object_show(ad->btn_level_1_text);
	evas_object_event_callback_add(ad->btn_level_1, EVAS_CALLBACK_MOUSE_DOWN, clicked_slight, ad);

	ad->btn_level_2 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_2, 255, 85, 85, 255);
	elm_grid_pack(ad->grid_main, ad->btn_level_2, 0, 50, 50, 50);
	evas_object_show(ad->btn_level_2);
	ad->btn_level_2_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_2_text, 255, 255, 255, 255);
	elm_object_text_set(ad->btn_level_2_text,
			"<align=center><font_size=30><b>보통 통증</b></font></align>");
	elm_grid_pack(ad->grid_main, ad->btn_level_2_text, 0, 65, 50, 10);
	evas_object_show(ad->btn_level_2_text);
	evas_object_event_callback_add(ad->btn_level_2, EVAS_CALLBACK_MOUSE_DOWN, clicked_slight, ad);

	ad->btn_level_3 = evas_object_rectangle_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_3, 62, 26, 23, 255);
	elm_grid_pack(ad->grid_main, ad->btn_level_3, 50, 50, 50, 50);
	evas_object_show(ad->btn_level_3);
	ad->btn_level_3_text = elm_label_add(ad->grid_main);
	evas_object_color_set(ad->btn_level_3_text, 255, 255, 255, 255);
	elm_object_text_set(ad->btn_level_3_text,
			"<align=center><font_size=30><b>강한 통증</b></font></align>");
	elm_grid_pack(ad->grid_main, ad->btn_level_3_text, 50, 65, 50, 10);
	evas_object_show(ad->btn_level_3_text);
	evas_object_event_callback_add(ad->btn_level_3, EVAS_CALLBACK_MOUSE_DOWN, clicked_slight, ad);

//	// slight button
//	ad->btn_slight = elm_button_add(ad->box_main);
//	evas_object_color_set(ad->btn_slight, 0, 235, 100, 255);
//	elm_object_style_set(ad->btn_slight, "default");
//	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
//	evas_object_size_hint_weight_set(ad->btn_slight, EVAS_HINT_EXPAND,
//	EVAS_HINT_EXPAND);
//	evas_object_size_hint_align_set(ad->btn_slight, EVAS_HINT_FILL,
//	EVAS_HINT_FILL);
//	elm_object_text_set(ad->btn_slight, "<b>SLIGHT</b>");
//	evas_object_show(ad->btn_slight);
//	elm_box_pack_start(ad->box_main, ad->btn_slight);
//
//	// slight button event
//	evas_object_smart_callback_add(ad->btn_slight, "clicked", clicked_slight,
//			ad);
//
//	// sharp button
//	ad->btn_sharp = elm_button_add(ad->box_main);
//	evas_object_color_set(ad->btn_sharp, 254, 71, 0, 255);
//	elm_object_style_set(ad->btn_slight, "default");
//	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
//	evas_object_size_hint_weight_set(ad->btn_sharp, EVAS_HINT_EXPAND,
//	EVAS_HINT_EXPAND);
//	evas_object_size_hint_align_set(ad->btn_sharp, EVAS_HINT_FILL,
//	EVAS_HINT_FILL);
//	elm_object_text_set(ad->btn_sharp, "<b>SHARP</b>");
//	evas_object_show(ad->btn_sharp);
//	elm_box_pack_end(ad->box_main, ad->btn_sharp);
//
//	// sharp button event
//	evas_object_smart_callback_add(ad->btn_sharp, "clicked", clicked_sharp, ad);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

/////////////////////////////////////////////

//QueryData* msgdata;
//
//static Eina_Bool
//layout_pop_cb(void *data, Elm_Object_Item *it)
//{
//    if(msgdata)
//	free(msgdata); /*need to free this structure in pop_cb of current layout*/
//    return EINA_TRUE;
//}

////////////////////////////////////////////

static bool app_create(void *data) {
	/* Hook to take necessary actions before main event loop starts
	 Initialize UI resources and application's data
	 If this function returns true, the main loop of application starts
	 If this function returns false, the application is terminated */
	int retval;
	bt_adapter_state_e bluetooth_adapter_state;

	appdata_s *ad = data;
	create_base_gui(ad);

	if (!check_hrm_sensor_is_supported()) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"A HRM sensor is not supported.");
		return false;
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG, "A HRM sensor is supported.");

	if (!check_physics_sensor_is_supported()) {
		dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
				"Physics sensor is not supported.");
		return false;
	} else
		dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
				"Physics sensors is supported.");

	if (!check_environment_sensor_is_supported()) {
		dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
				"Environment sensor is not supported.");
		return false;
	} else
		dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
				"Environment sensors is supported.");

	// Bluetooth //
	retval = bt_initialize();
	if (retval != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Function bt_initialize() return value = %s",
				get_error_message(retval));
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to initialize the Bluetooth API.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in initializing the Bluetooth API.");
	retval = bt_adapter_get_state(&bluetooth_adapter_state);
	if (retval != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Function bt_adapter_get_state() return value = %s",
				get_error_message(retval));
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to get the current state of local Bluetooth adapter.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in getting the current state of local Bluetooth adapter.");
	if (bluetooth_adapter_state == BT_ADAPTER_DISABLED) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Bluetooth adapter is disabled.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Bluetooth adapter is enabled.");
	retval = bt_gatt_server_initialize();
	if (retval != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Function bt_gatt_server_initialize() return value = %s",
				get_error_message(retval));
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to initialize the GATT server.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in initializing the GATT server.");
	if (!create_gatt_descriptor()) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to create the GATT characteristic descriptor.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in creating the GATT characteristic descriptor.");
	if (!create_gatt_characteristic()) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to create the GATT characteristic.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in creating the GATT characteristic.");
	if (!create_gatt_service()) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to create the GATT service.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in creating the GATT service.");
	if (!create_gatt_server()) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to create the GATT server's handle.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in creating the GATT server's handle.");
	if (!start_gatt_server()) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to register the application along with the GATT services of the application it is hosting.");
//			return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in registering the application along with the GATT services of the application it is hosting.");
	if (!create_bluetooth_le_advertiser()) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to create advertiser to advertise device's existence.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in creating advertiser to advertise device's existence.");
	if (!start_bluetooth_le_advertising()) {
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to start advertising with passed advertiser and advertising parameters.");
//		return false;
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in starting advertising with passed advertiser and advertising parameters.");

//	int ret = initdb();
//	dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "Init Sqlite : %i", ret);

//	/*allocate msgdata memory. this will be used for retrieving data from database*/
//	msgdata = (QueryData*) calloc (1, sizeof(QueryData));
//
//	int num_of_rows = 0;
//	int num_of_rows2 = 0;
//	/*retrieve all msgdata from database*/
//	int ret2 = getTotalMsgItemsCount(&num_of_rows2);
//	if(!ret2){
//		dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "Total rows found: [%d]", num_of_rows2);
//	} else {
//		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "row count error!");
//	}
//
//
//	int ret = getAllMsgFromDb(&msgdata, &num_of_rows); /*sending msgdata reference to get data from database; num_of_rows reference  to get count of rows*/
//	if(!ret){
//		dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "Retrieved [%d] rows successfully!", num_of_rows);
//	} else {
//		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "Data retrieval error!");
//	}
//	dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "Check Size of sqlite : %s", msgdata->msg);

//	char* buf = "This is Test Code. Hi.";
//	char *filepath;
//	filepath=get_write_filepath("text.txt"); // "text.txt" is file name
//	dlog_print(DLOG_WARN, SQLITE3_LOG_TAG, "CHECK [%s]", filepath);
//	write_file(filepath,buf);
//
//	char* temp = read_file(filepath);
//	dlog_print(DLOG_WARN, SQLITE3_LOG_TAG, "READ [%s]", temp);

//	int ret = sqlite3_check_useable_and_open(sql_db, "sample.db");
//	dlog_print(DLOG_WARN, SQLITE3_LOG_TAG, "check: %i", ret);
//	sqlitehelper_open();

//	char * filepath = get_write_filepath("hda_sensor_data.txt");
//	read_file(filepath);

	return true;
}

static void app_control(app_control_h app_control, void *data) {
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
	if (!check_and_request_sensor_permission()) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Failed to check if an application has permission to use the sensor privilege.");
		ui_app_exit();
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Succeeded in checking if an application has permission to use the sensor privilege.");

}

static void app_terminate(void *data) {
	/* Release all resources. */
	int retval;

	if (check_hrm_sensor_listener_is_created()) {
		if (!destroy_hrm_sensor_listener())
			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
					"Failed to release all the resources allocated for a HRM sensor listener.");
		else
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"Succeeded in releasing all the resources allocated for a HRM sensor listener.");
	}

	if (check_physics_sensor_listener_is_created()) {
		if (!destroy_physics_sensor_listener())
			dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
					"Failed to release all the resources allocated for a Physics sensor listener.");
		else
			dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
					"Succeeded in releasing all the resources allocated for a Physics sensor listener.");
	}

	if (check_environment_sensor_listener_is_created()) {
		if (!destroy_environment_sensor_listener())
			dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
					"Failed to release all the resources allocated for a Environment sensor listener.");
		else
			dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
					"Succeeded in releasing all the resources allocated for a Environment sensor listener.");
	}

	// Bluetooth //
	if (!destroy_gatt_service())
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to destroy the GATT handle of service.");
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in destroying the GATT handle of service.");
	if (!destroy_gatt_server())
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG,
				"Failed to destroy the GATT server's handle.");
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in destroying the GATT server's handle.");

	retval = bt_deinitialize();
	if (retval != BT_ERROR_NONE) {
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Function bt_deinitialize() return value = %s",
				get_error_message(retval));
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Failed to release all resources of the Bluetooth API.");
	} else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG,
				"Succeeded in releasing all resources of the Bluetooth API.");
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE,
			&locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_BATTERY*/
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char *argv[]) {
	appdata_s ad = { 0, };
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}

//////////////////////////////////////////////////////////////////
///////////////////////////// Screen /////////////////////////////
//////////////////////////////////////////////////////////////////

static void activated_screen(appdata_s *ad) {
	ad->bg = elm_bg_add(ad->nv);
	elm_bg_color_set(ad->bg, 255, 255, 255);
	elm_object_content_set(ad->conform, ad->bg);

//	ad->box_main = elm_box_add(ad->bg);
//	elm_box_horizontal_set(ad->box_main, EINA_TRUE);
//	elm_box_padding_set(ad->box_main, 0, 0);
//	elm_object_content_set(ad->bg, ad->box_main);
//	evas_object_show(ad->box_main);

//	ad->nv_it = elm_naviframe_item_push(ad->nv, NULL, NULL, NULL, ad->bg, NULL);
//	elm_naviframe_item_title_enabled_set(ad->nv_it, EINA_FALSE, EINA_FALSE);

//	// slight button
//	ad->btn_slight = elm_button_add(ad->box_main);
//	evas_object_color_set(ad->btn_slight, 0, 235, 100, 255);
//	elm_object_style_set(ad->btn_slight, "default");
//	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
//	evas_object_size_hint_weight_set(ad->btn_slight, EVAS_HINT_EXPAND,
//	EVAS_HINT_EXPAND);
//	evas_object_size_hint_align_set(ad->btn_slight, EVAS_HINT_FILL,
//	EVAS_HINT_FILL);
//	elm_object_text_set(ad->btn_slight, "<b>SLIGHT</b>");
//	evas_object_show(ad->btn_slight);
//	elm_box_pack_start(ad->box_main, ad->btn_slight);
//
//	// slight button event
//	evas_object_smart_callback_add(ad->btn_slight, "clicked", clicked_slight,
//			ad);
//
//	// sharp button
//	ad->btn_sharp = elm_button_add(ad->box_main);
//	evas_object_color_set(ad->btn_sharp, 254, 71, 0, 255);
//	elm_object_style_set(ad->btn_slight, "default");
//	evas_object_size_hint_min_set(ad->btn_slight, EVAS_HINT_EXPAND, 500);
//	evas_object_size_hint_weight_set(ad->btn_sharp, EVAS_HINT_EXPAND,
//	EVAS_HINT_EXPAND);
//	evas_object_size_hint_align_set(ad->btn_sharp, EVAS_HINT_FILL,
//	EVAS_HINT_FILL);
//	elm_object_text_set(ad->btn_sharp, "<b>SHARP</b>");
//	evas_object_show(ad->btn_sharp);
//	elm_box_pack_end(ad->box_main, ad->btn_sharp);
//
//	// sharp button event
//	evas_object_smart_callback_add(ad->btn_sharp, "clicked", clicked_sharp, ad);

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

static void clicked_slight(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	dlog_print(DLOG_INFO, "HDA_EVENT", "Slight");

	Evas_Object *popup;
	appdata_s *ad = user_data;
	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "toast");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			eext_popup_back_cb, NULL);
	elm_object_text_set(popup, "Clicked slight");
	elm_popup_timeout_set(popup, 2.0);
	evas_object_show(popup);
}
static void clicked_sharp(void *user_data, Evas* e, Evas_Object *obj,
		void *event_info) {
	dlog_print(DLOG_INFO, "HDA_EVENT", "Sharp");

	Evas_Object *popup;
	appdata_s *ad = user_data;
	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "toast");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			eext_popup_back_cb, NULL);
	elm_object_text_set(popup, "Clicked sharp");
	elm_popup_timeout_set(popup, 3.0);
	evas_object_show(popup);
}

//sensor_is_supported()
bool check_hrm_sensor_is_supported() {
	int hrm_retval;
	bool hrm_supported = false;
	hrm_retval = sensor_is_supported(SENSOR_HRM, &hrm_supported);

	if (hrm_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Function sensor_is_supported() return value = %s",
				get_error_message(hrm_retval));
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Failed to checks whether a HRM sensor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Succeeded in checking whether a HRM sensor is supported in the current device.");

	if (!hrm_supported) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Function sensor_is_supported() output supported = %d",
				hrm_supported);
		return false;
	} else
		return true;
}

bool check_physics_sensor_is_supported() {
	int accelerometer_retval;
	bool accelerometer_supported = false;
	accelerometer_retval = sensor_is_supported(SENSOR_ACCELEROMETER,
			&accelerometer_supported);

	if (accelerometer_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, ACCELEROMETER_SENSOR_LOG_TAG,
				"Accelerometer sensor_is_supported() return value = %s",
				get_error_message(accelerometer_retval));
		dlog_print(DLOG_ERROR, ACCELEROMETER_SENSOR_LOG_TAG,
				"Failed to checks whether a Accelerometer sensor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG,
				"Succeeded in checking whether a Accelerometer sensor is supported in the current device.");

	if (!accelerometer_supported) {
		dlog_print(DLOG_ERROR, ACCELEROMETER_SENSOR_LOG_TAG,
				"Accelerometer sensor_is_supported() output supported = %d",
				accelerometer_supported);
		return false;
	}

	int gravity_retval;
	bool gravity_supported = false;
	gravity_retval = sensor_is_supported(SENSOR_GRAVITY, &gravity_supported);

	if (gravity_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, GRAVITY_SENSOR_LOG_TAG,
				"Gravity sensor_is_supported() return value = %s",
				get_error_message(gravity_retval));
		dlog_print(DLOG_ERROR, GRAVITY_SENSOR_LOG_TAG,
				"Failed to checks whether a Gravity sensor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, GRAVITY_SENSOR_LOG_TAG,
				"Succeeded in checking whether a Gravity sensor is supported in the current device.");

	if (!gravity_supported) {
		dlog_print(DLOG_ERROR, GRAVITY_SENSOR_LOG_TAG,
				"Gravity sensor_is_supported() output supported = %d",
				gravity_supported);
		return false;
	}

	int gyroscope_rotation_vector_retval;
	bool gyroscope_rotation_vector_supported = false;
	gyroscope_rotation_vector_retval = sensor_is_supported(SENSOR_GYROSCOPE,
			&gyroscope_rotation_vector_supported);

	if (gyroscope_rotation_vector_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, GYROSCOPE_ROTATION_VECTOR_SENSOR_LOG_TAG,
				"Gyroscope Rotation Vector sensor_is_supported() return value = %s",
				get_error_message(gyroscope_rotation_vector_retval));
		dlog_print(DLOG_ERROR, GYROSCOPE_ROTATION_VECTOR_SENSOR_LOG_TAG,
				"Failed to checks whether a Gyroscope Rotation Vector is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, GYROSCOPE_ROTATION_VECTOR_SENSOR_LOG_TAG,
				"Succeeded in checking whether a Gyroscope Rotation Vector sensor is supported in the current device.");

	if (!gyroscope_rotation_vector_supported) {
		dlog_print(DLOG_ERROR, GYROSCOPE_ROTATION_VECTOR_SENSOR_LOG_TAG,
				"Gyroscope sensor_is_supported() output supported = %d",
				gyroscope_rotation_vector_supported);
		return false;
	}

	int gyroscope_retval;
	bool gyroscope_supported = false;
	gyroscope_retval = sensor_is_supported(SENSOR_GYROSCOPE,
			&gyroscope_supported);

	if (gyroscope_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, GYROSCOPE_SENSOR_LOG_TAG,
				"Gyroscope sensor_is_supported() return value = %s",
				get_error_message(gyroscope_retval));
		dlog_print(DLOG_ERROR, GYROSCOPE_SENSOR_LOG_TAG,
				"Failed to checks whether a Gyroscope sensor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, GYROSCOPE_SENSOR_LOG_TAG,
				"Succeeded in checking whether a Gyroscope sensor is supported in the current device.");

	if (!gyroscope_supported) {
		dlog_print(DLOG_ERROR, GYROSCOPE_SENSOR_LOG_TAG,
				"Gyroscope sensor_is_supported() output supported = %d",
				gyroscope_supported);
		return false;
	}

	int linear_acceleration_retval;
	bool linear_acceleration_supported = false;
	linear_acceleration_retval = sensor_is_supported(SENSOR_LINEAR_ACCELERATION,
			&linear_acceleration_supported);

	if (linear_acceleration_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LINEAR_ACCELERATION_SENSOR_LOG_TAG,
				"linear_acceleration sensor_is_supported() return value = %s",
				get_error_message(linear_acceleration_retval));
		dlog_print(DLOG_ERROR, LINEAR_ACCELERATION_SENSOR_LOG_TAG,
				"Failed to checks whether a linear_acceleration sensor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, LINEAR_ACCELERATION_SENSOR_LOG_TAG,
				"Succeeded in checking whether a linear_acceleration sensor is supported in the current device.");

	if (!linear_acceleration_supported) {
		dlog_print(DLOG_ERROR, LINEAR_ACCELERATION_SENSOR_LOG_TAG,
				"linear_acceleration sensor_is_supported() output supported = %d",
				linear_acceleration_supported);
		return false;
	}

	return true;
}

bool check_environment_sensor_is_supported() {
	int light_retval;
	bool light_supported = false;
	light_retval = sensor_is_supported(SENSOR_LIGHT, &light_supported);

	if (light_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"Function sensor_is_supported() return value = %s",
				get_error_message(light_retval));
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"Failed to checks whether a Light sensor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
				"Succeeded in checking whether a Light sensor is supported in the current device.");

	if (!light_supported) {
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"Function sensor_is_supported() output supported = %d",
				light_supported);
		return false;
	}

	int pedometer_retval;
	bool pedometer_supported = false;
	pedometer_retval = sensor_is_supported(SENSOR_HUMAN_PEDOMETER,
			&pedometer_supported);

	if (pedometer_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"Function sensor_is_supported() return value = %s",
				get_error_message(pedometer_retval));
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"Failed to checks whether a Pedometer is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"Succeeded in checking whether a Pedometer is supported in the current device.");

	if (!pedometer_supported) {
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"Function sensor_is_supported() output supported = %d",
				pedometer_supported);
		return false;
	}

	int pressure_retval;
	bool pressure_supported = false;
	pressure_retval = sensor_is_supported(SENSOR_PRESSURE, &pressure_supported);

	if (pressure_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"Function sensor_is_supported() return value = %s",
				get_error_message(pressure_retval));
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"Failed to checks whether a Pressure sensor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"Succeeded in checking whether a Pressure sensor is supported in the current device.");

	if (!pressure_supported) {
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"Function sensor_is_supported() output supported = %d",
				pressure_supported);
		return false;
	}

	int sleep_monitor_retval;
	bool sleep_monitor_supported = false;
	sleep_monitor_retval = sensor_is_supported(SENSOR_HUMAN_SLEEP_MONITOR,
			&sleep_monitor_supported);

	if (sleep_monitor_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"Function sensor_is_supported() return value = %s",
				get_error_message(sleep_monitor_retval));
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"Failed to checks whether a sleep_monitor is supported in the current device.");
		return false;
	} else
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"Succeeded in checking whether a sleep_monitor sensor is supported in the current device.");

	if (!sleep_monitor_supported) {
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"Function sensor_is_supported() output supported = %d",
				sleep_monitor_supported);
		return false;
	}
	return true;
}

//initialize_sensor()
bool initialize_hrm_sensor() {
	int retval;

	retval = sensor_get_default_sensor(SENSOR_HRM, &hrm_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Function sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else
		return true;
}

bool initialize_hrm_led_green_sensor() {
	int retval;

	retval = sensor_get_default_sensor(SENSOR_HRM_LED_GREEN,
			&hrm_led_green_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"HRM LED green sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else
		return true;
}

bool initialize_accelerometer_sensor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_ACCELEROMETER,
			&accelerometer_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, ACCELEROMETER_SENSOR_LOG_TAG,
				"Accelerometer sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG,
				"Accelerometer initialized.");
		return true;
	}
}

bool initialize_gravity_sensor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_GRAVITY, &gravity_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, GRAVITY_SENSOR_LOG_TAG,
				"Gravity sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, GRAVITY_SENSOR_LOG_TAG, "Gravity initialized.");
		return true;
	}
}

bool initialize_gyroscope_rotation_vector_sensor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_GYROSCOPE_ROTATION_VECTOR,
			&gyroscope_rotation_vector_sensor_hanlde);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, GYROSCOPE_ROTATION_VECTOR_SENSOR_LOG_TAG,
				"Gyroscope rotation sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, GYROSCOPE_ROTATION_VECTOR_SENSOR_LOG_TAG,
				"Gyroscope rotation initialized.");
		return true;
	}
}

bool initialize_gyroscope_sensor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_GYROSCOPE,
			&gyroscope_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, GYROSCOPE_SENSOR_LOG_TAG,
				"Gyroscope sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, GYROSCOPE_SENSOR_LOG_TAG,
				"Gyroscope initialized.");
		return true;
	}
}

bool initialize_linear_acceleration_sensor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_LINEAR_ACCELERATION,
			&linear_acceleration_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LINEAR_ACCELERATION_SENSOR_LOG_TAG,
				"linear_acceleration sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, LINEAR_ACCELERATION_SENSOR_LOG_TAG,
				"linear_acceleration initialized.");
		return true;
	}
}

bool initialize_light_sensor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_LIGHT, &light_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"Light sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG, "Light initialized.");
		return true;
	}
}

bool initialize_pedometer() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_HUMAN_PEDOMETER,
			&pedometer_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"Pedometer sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG, "Pedometer initialized.");
		return true;
	}
}

bool initialize_pressure_sensor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_PRESSURE,
			&pressure_sensor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"pressure sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"pressure_sensor initialized.");
		return true;
	}
}

bool initialize_sleep_monitor() {
	int retval;
	retval = sensor_get_default_sensor(SENSOR_HUMAN_SLEEP_MONITOR,
			&sleep_monitor_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"sleep_monitor sensor_get_default_sensor() return value = %s",
				get_error_message(retval));
		return false;
	} else {
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"sleep_monitor initialized.");
		return true;
	}
}

bool check_and_request_sensor_permission() {
	bool health_usable = true;
	bool physics_usable = true;
	bool environment_usable = true;

	int health_retval;
	int mediastorage_retval;
	ppm_check_result_e health_result;
	ppm_check_result_e mediastorage_result;

	health_retval = ppm_check_permission(sensor_privilege, &health_result);
	mediastorage_retval = ppm_check_permission(mediastorage_privilege,
			&mediastorage_result);

	if (health_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		switch (health_result) {
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"The application has permission to use a sensor privilege.");

			if (!check_hrm_sensor_listener_is_created()) {
				if (!initialize_hrm_sensor()
						|| !initialize_hrm_led_green_sensor()) {
					dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
							"Failed to get the handle for the default sensor of a HRM sensor.");
					health_usable = false;
				} else
					dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
							"Succeeded in getting the handle for the default sensor of a HRM sensor.");

				if (!create_hrm_sensor_listener(hrm_sensor_handle,
						hrm_led_green_sensor_handle)) {
					dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
							"Failed to create a HRM sensor listener.");
					health_usable = false;
				} else
					dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
							"Succeeded in creating a HRM sensor listener.");

				if (!start_hrm_sensor_listener())
					dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
							"Failed to start observing the sensor events regarding a HRM sensor listener.");
				else
					dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
							"Succeeded in starting observing the sensor events regarding a HRM sensor listener.");
			}
			break;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
			/* Show a message and terminate the application */
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY");
			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
					"The application doesn't have permission to use a sensor privilege.");
			health_usable = false;
			break;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"The user has to be asked whether to grant permission to use a sensor privilege.");

			if (!request_sensor_permission()) {
				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
						"Failed to request a user's response to obtain permission for using the sensor privilege.");
				health_usable = false;
			} else {
				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
						"Succeeded in requesting a user's response to obtain permission for using the sensor privilege.");
				health_usable = true;
			}
			break;
		}
	} else {
		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
		/* Handle errors */
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Function ppm_check_permission() return %s",
				get_error_message(health_retval));
		health_usable = false;
	}

	if (mediastorage_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		if (mediastorage_result
				== PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW) {
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"The application has permission to use a storage privilege.");

			if (!check_physics_sensor_listener_is_created()) {
				if (!initialize_accelerometer_sensor()
						|| !initialize_gravity_sensor()
						|| !initialize_gyroscope_rotation_vector_sensor()
						|| !initialize_gyroscope_sensor()
						|| !initialize_linear_acceleration_sensor()) {
					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
							"Failed to get the handle for the default sensor of a Physics sensor.");
					physics_usable = false;
				} else
					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
							"Succeeded in getting the handle for the default sensor of a Physics sensor.");

				if (!create_physics_sensor_listener(accelerometer_sensor_handle,
						gravity_sensor_handle,
						gyroscope_rotation_vector_sensor_hanlde,
						gyroscope_sensor_handle,
						linear_acceleration_sensor_handle)) {
					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
							"Failed to create a Physics sensor listener.");
					physics_usable = false;
				} else
					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
							"Succeeded in creating a Physics sensor listener.");

				if (!start_physics_sensor_listener())
					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
							"Failed to start observing the sensor events regarding a Physics sensor listener.");
				else
					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
							"Succeeded in starting observing the sensor events regarding a Physics sensor listener.");
			}

			if (!check_environment_sensor_listener_is_created()) {
				if (!initialize_light_sensor() || !initialize_pedometer()
						|| !initialize_pressure_sensor()
						|| !initialize_sleep_monitor()) {
					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
							"Failed to get the handle for the default sensor of a Environment sensor.");
					environment_usable = false;
				} else
					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
							"Succeeded in getting the handle for the default sensor of a Environment sensor.");

				if (!create_environment_sensor_listener(light_sensor_handle,
						pedometer_handle, pressure_sensor_handle,
						sleep_monitor_handle)) {
					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
							"Failed to create a Environment sensor listener.");
					environment_usable = false;
				} else
					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
							"Succeeded in creating a Environment sensor listener.");

				if (!start_environment_sensor_listener())
					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
							"Failed to start observing the sensor events regarding a Environment sensor listener.");
				else
					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
							"Succeeded in starting observing the sensor events regarding a Environment sensor listener.");
			}
		} else if (mediastorage_result
				== PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK) {
			dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
					"The user has to be asked whether to grant permission to use a sensor privilege.");

			if (!request_sensor_permission()) {
				dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
						"Failed to request a user's response to obtain permission for using the sensor privilege.");
				physics_usable = false;
				environment_usable = false;
			} else {
				dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
						"Succeeded in requesting a user's response to obtain permission for using the sensor privilege.");
				physics_usable = true;
				environment_usable = true;
			}
		} else {
			dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
					"Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY");
			dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
					"The application doesn't have permission to use a sensor privilege.");
			physics_usable = false;
			environment_usable = false;
		}
	} else {
		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
		/* Handle errors */
		dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
				"Function ppm_check_permission() return %s",
				get_error_message(mediastorage_retval));
		physics_usable = false;
		environment_usable = false;
	}

	return health_usable && physics_usable && environment_usable;

//	int retval;
//	ppm_check_result_e result;
//	health_retval = ppm_check_permission(sensor_privilege, &result);
//
//	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
//		switch (result) {
//		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
//			/* Update UI and start accessing protected functionality */
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"The application has permission to use a sensor privilege.");
//
//			if (!check_hrm_sensor_listener_is_created()) {
//				if (!initialize_hrm_sensor()
//						|| !initialize_hrm_led_green_sensor()) {
//					dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//							"Failed to get the handle for the default sensor of a HRM sensor.");
//					return false;
//				} else
//					dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//							"Succeeded in getting the handle for the default sensor of a HRM sensor.");
//
//				if (!create_hrm_sensor_listener(hrm_sensor_handle,
//						hrm_led_green_sensor_handle)) {
//					dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//							"Failed to create a HRM sensor listener.");
//					//return false;
//				} else
//					dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//							"Succeeded in creating a HRM sensor listener.");
//
//				if (!start_hrm_sensor_listener())
//					dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//							"Failed to start observing the sensor events regarding a HRM sensor listener.");
//				else
//					dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//							"Succeeded in starting observing the sensor events regarding a HRM sensor listener.");
//			}
//
//			if (!check_physics_sensor_listener_is_created()) {
//				if (!initialize_accelerometer_sensor()
//						|| !initialize_gravity_sensor()
//						|| !initialize_gyroscope_rotation_vector_sensor()
//						|| !initialize_gyroscope_sensor()
//						|| !initialize_linear_acceleration_sensor()) {
//					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
//							"Failed to get the handle for the default sensor of a Physics sensor.");
//					return false;
//				} else
//					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
//							"Succeeded in getting the handle for the default sensor of a Physics sensor.");
//
//				if (!create_physics_sensor_listener(accelerometer_sensor_handle,
//						gravity_sensor_handle,
//						gyroscope_rotation_vector_sensor_hanlde,
//						gyroscope_sensor_handle,
//						linear_acceleration_sensor_handle)) {
//					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
//							"Failed to create a Physics sensor listener.");
//					return false;
//				} else
//					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
//							"Succeeded in creating a Physics sensor listener.");
//
//				if (!start_physics_sensor_listener())
//					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
//							"Failed to start observing the sensor events regarding a Physics sensor listener.");
//				else
//					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
//							"Succeeded in starting observing the sensor events regarding a Physics sensor listener.");
//			}
//
//			if (!check_environment_sensor_listener_is_created()) {
//				if (!initialize_light_sensor() || !initialize_pedometer()
//						|| !initialize_pressure_sensor()
//						|| !initialize_sleep_monitor()) {
//					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Failed to get the handle for the default sensor of a Environment sensor.");
//					return false;
//				} else
//					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Succeeded in getting the handle for the default sensor of a Environment sensor.");
//
//				if (!create_environment_sensor_listener(light_sensor_handle,
//						pedometer_handle, pressure_sensor_handle,
//						sleep_monitor_handle)) {
//					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Failed to create a Environment sensor listener.");
//					return false;
//				} else
//					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Succeeded in creating a Environment sensor listener.");
//
//				if (!start_environment_sensor_listener())
//					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Failed to start observing the sensor events regarding a Environment sensor listener.");
//				else
//					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Succeeded in starting observing the sensor events regarding a Environment sensor listener.");
//			}
//			return true;
//		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
//			/* Show a message and terminate the application */
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY");
//			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//					"The application doesn't have permission to use a sensor privilege.");
//			return false;
//		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"The user has to be asked whether to grant permission to use a sensor privilege.");
//
//			if (!request_sensor_permission()) {
//				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//						"Failed to request a user's response to obtain permission for using the sensor privilege.");
//				return false;
//			} else {
//				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//						"Succeeded in requesting a user's response to obtain permission for using the sensor privilege.");
//				return true;
//			}
//		}
//	} else {
//		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
//		/* Handle errors */
//		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//				"Function ppm_check_permission() return %s",
//				get_error_message(retval));
//		return false;
//	}
}

bool request_sensor_permission() {
	int health_retval;
	int mediastorage_retval;

	health_retval = ppm_request_permission(sensor_privilege,
			request_health_sensor_permission_response_callback, NULL);
	mediastorage_retval = ppm_request_permission(mediastorage_privilege,
			request_physics_sensor_permission_response_callback, NULL);

	if (health_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE
			&& mediastorage_retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		return true;
	} else if (health_retval
			== PRIVACY_PRIVILEGE_MANAGER_ERROR_ALREADY_IN_PROGRESS
			&& mediastorage_retval
					== PRIVACY_PRIVILEGE_MANAGER_ERROR_ALREADY_IN_PROGRESS) {
		return true;
	} else {
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Function ppm_request_permission() return value = %s",
				get_error_message(health_retval));
		dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
				"Function ppm_request_permission() return value = %s",
				get_error_message(mediastorage_retval));
		return false;
	}

//	int retval;
//
//	retval = ppm_request_permission(sensor_privilege,
//			request_sensor_permission_response_callback, NULL);
//
//	/* Log and handle errors */
//	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
//		return true;
//	else if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_ALREADY_IN_PROGRESS)
//		return true;
//	else {
//		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//				"Function ppm_request_permission() return value = %s",
//				get_error_message(retval));
//		return false;
//	}
}

void request_health_sensor_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data) {
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR) {
		/* Log and handle errors */
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR");
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() was called because of an error.");
	} else {
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() was called with a valid answer.");

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"The user granted permission to use a sensor privilege for an indefinite period of time.");

			if (!initialize_hrm_sensor()
					|| !initialize_hrm_led_green_sensor()) {
				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
						"Failed to get the handle for the default sensor of a HRM sensor.");
				ui_app_exit();
			} else
				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
						"Succeeded in getting the handle for the default sensor of a HRM sensor.");

			if (!create_hrm_sensor_listener(hrm_sensor_handle,
					hrm_led_green_sensor_handle)) {
				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
						"Failed to create a HRM sensor listener.");
				ui_app_exit();
			} else
				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
						"Succeeded in creating a HRM sensor listener.");

			if (!start_hrm_sensor_listener()) {
				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
						"Failed to start observing the sensor events regarding a HRM sensor listener.");
				ui_app_exit();
			} else
				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
						"Succeeded in starting observing the sensor events regarding a HRM sensor listener.");
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
			/* Show a message and terminate the application */
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER");
			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
					"The user denied granting permission to use a sensor privilege for an indefinite period of time.");
			ui_app_exit();
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
			/* Show a message with explanation */
			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
					"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE");
			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
					"The user denied granting permission to use a sensor privilege once.");
			ui_app_exit();
			break;
		}
	}
}

void request_physics_sensor_permission_response_callback(ppm_call_cause_e cause,
		ppm_request_result_e result, const char *privilege, void *user_data) {
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR) {
		/* Log and handle errors */
		dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR");
		dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() was called because of an error.");
	} else {
		dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
				"Function request_sensor_permission_response_callback() was called with a valid answer.");

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
					"The user granted permission to use a sensor privilege for an indefinite period of time.");

			if (!initialize_accelerometer_sensor()
					|| !initialize_gravity_sensor()
					|| !initialize_gyroscope_rotation_vector_sensor()
					|| !initialize_gyroscope_sensor()
					|| !initialize_linear_acceleration_sensor()) {
				dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
						"Failed to get the handle for the default sensor of a Physics sensor.");
			} else
				dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
						"Succeeded in getting the handle for the default sensor of a Physics sensor.");

			if (!create_physics_sensor_listener(accelerometer_sensor_handle,
					gravity_sensor_handle,
					gyroscope_rotation_vector_sensor_hanlde,
					gyroscope_sensor_handle,
					linear_acceleration_sensor_handle)) {
				dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
						"Failed to create a Physics sensor listener.");
			} else
				dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
						"Succeeded in creating a Physics sensor listener.");

			if (!start_physics_sensor_listener())
				dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
						"Failed to start observing the sensor events regarding a Physics sensor listener.");
			else
				dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
						"Succeeded in starting observing the sensor events regarding a Physics sensor listener.");

			if (!initialize_light_sensor() || !initialize_pedometer()
					|| !initialize_pressure_sensor()
					|| !initialize_sleep_monitor()) {
				dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
						"Failed to get the handle for the default sensor of a Environment sensor.");
			} else
				dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
						"Succeeded in getting the handle for the default sensor of a Environment sensor.");

			if (!create_environment_sensor_listener(light_sensor_handle,
					pedometer_handle, pressure_sensor_handle,
					sleep_monitor_handle)) {
				dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
						"Failed to create a Environment sensor listener.");
			} else
				dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
						"Succeeded in creating a Environment sensor listener.");

			if (!start_environment_sensor_listener())
				dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
						"Failed to start observing the sensor events regarding a Environment sensor listener.");
			else
				dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
						"Succeeded in starting observing the sensor events regarding a Environment sensor listener.");

			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
			/* Show a message and terminate the application */
			dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
					"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER");
			dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
					"The user denied granting permission to use a sensor privilege for an indefinite period of time.");
			ui_app_exit();
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
			/* Show a message with explanation */
			dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
					"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE");
			dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
					"The user denied granting permission to use a sensor privilege once.");
			ui_app_exit();
			break;
		}
	}
}

//void request_sensor_permission_response_callback(ppm_call_cause_e cause,
//		ppm_request_result_e result, const char *privilege, void *user_data) {
//	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR) {
//		/* Log and handle errors */
//		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//				"Function request_sensor_permission_response_callback() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR");
//		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//				"Function request_sensor_permission_response_callback() was called because of an error.");
//	} else {
//		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//				"Function request_sensor_permission_response_callback() was called with a valid answer.");
//
//		switch (result) {
//		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
//			/* Update UI and start accessing protected functionality */
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"The user granted permission to use a sensor privilege for an indefinite period of time.");
//
//			if (!initialize_hrm_sensor()
//					|| !initialize_hrm_led_green_sensor()) {
//				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//						"Failed to get the handle for the default sensor of a HRM sensor.");
//				ui_app_exit();
//			} else
//				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//						"Succeeded in getting the handle for the default sensor of a HRM sensor.");
//
//			if (!create_hrm_sensor_listener(hrm_sensor_handle,
//					hrm_led_green_sensor_handle)) {
//				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//						"Failed to create a HRM sensor listener.");
//				ui_app_exit();
//			} else
//				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//						"Succeeded in creating a HRM sensor listener.");
//
//			if (!start_hrm_sensor_listener()) {
//				dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//						"Failed to start observing the sensor events regarding a HRM sensor listener.");
//				ui_app_exit();
//			} else
//				dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//						"Succeeded in starting observing the sensor events regarding a HRM sensor listener.");
//
//			if (!check_physics_sensor_listener_is_created()) {
//				if (!initialize_accelerometer_sensor()
//						|| !initialize_gravity_sensor()
//						|| !initialize_gyroscope_rotation_vector_sensor()
//						|| !initialize_gyroscope_sensor()
//						|| !initialize_linear_acceleration_sensor()) {
//					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
//							"Failed to get the handle for the default sensor of a Physics sensor.");
//				} else
//					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
//							"Succeeded in getting the handle for the default sensor of a Physics sensor.");
//
//				if (!create_physics_sensor_listener(accelerometer_sensor_handle,
//						gravity_sensor_handle,
//						gyroscope_rotation_vector_sensor_hanlde,
//						gyroscope_sensor_handle,
//						linear_acceleration_sensor_handle)) {
//					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
//							"Failed to create a Physics sensor listener.");
//				} else
//					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
//							"Succeeded in creating a Physics sensor listener.");
//
//				if (!start_physics_sensor_listener())
//					dlog_print(DLOG_ERROR, PHYSICS_SENSOR_LOG_TAG,
//							"Failed to start observing the sensor events regarding a Physics sensor listener.");
//				else
//					dlog_print(DLOG_INFO, PHYSICS_SENSOR_LOG_TAG,
//							"Succeeded in starting observing the sensor events regarding a Physics sensor listener.");
//			}
//			if (!check_environment_sensor_listener_is_created()) {
//				if (!initialize_light_sensor() || !initialize_pedometer()
//						|| !initialize_pressure_sensor()
//						|| !initialize_sleep_monitor()) {
//					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Failed to get the handle for the default sensor of a Environment sensor.");
//				} else
//					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Succeeded in getting the handle for the default sensor of a Environment sensor.");
//
//				if (!create_environment_sensor_listener(light_sensor_handle,
//						pedometer_handle, pressure_sensor_handle,
//						sleep_monitor_handle)) {
//					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Failed to create a Environment sensor listener.");
//				} else
//					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Succeeded in creating a Environment sensor listener.");
//
//				if (!start_environment_sensor_listener())
//					dlog_print(DLOG_ERROR, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Failed to start observing the sensor events regarding a Environment sensor listener.");
//				else
//					dlog_print(DLOG_INFO, ENVIRONMENT_SENSOR_LOG_TAG,
//							"Succeeded in starting observing the sensor events regarding a Environment sensor listener.");
//			}
//			break;
//		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
//			/* Show a message and terminate the application */
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER");
//			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//					"The user denied granting permission to use a sensor privilege for an indefinite period of time.");
//			ui_app_exit();
//			break;
//		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
//			/* Show a message with explanation */
//			dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
//					"Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE");
//			dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
//					"The user denied granting permission to use a sensor privilege once.");
//			ui_app_exit();
//			break;
//		}
//	}
//}


static Eina_Bool _do_animation(void *data, double pos)
{
    Evas_Object *dest = data;
//    evas_object_color_set(dest, 50, 50, 50, 255);
//    evas_object_size_hint_min_set(rect, 100, 230*pos);
    return ECORE_CALLBACK_RENEW;
}

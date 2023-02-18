#include <sensor/hrm_listener.h>
#include "hda_service.h"
#include "bluetooth/gatt/characteristic.h"
#include <tools/sqlite_helper.h>

sensor_listener_h hrm_sensor_listener_handle = 0;
sensor_listener_h hrm_led_green_sensor_listener_handle = 0;

unsigned int hrm_sensor_listener_event_update_interval_ms = 1000;

static void hrm_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], void *user_data);
static void hrm_led_green_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], void *user_data);

bool create_hrm_sensor_listener(sensor_h hrm_sensor_handle,
		sensor_h hrm_led_green_sensor_handle) {
	int retval;

	retval = sensor_create_listener(hrm_sensor_handle,
			&hrm_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_create_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	if (!set_hrm_sensor_listener_attribute()) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to set an attribute to control the behavior of a HRM sensor listener.",
				__FILE__, __func__, __LINE__);
		return false;
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in setting an attribute to control the behavior of a HRM sensor listener.",
				__FILE__, __func__, __LINE__);

	if (!set_hrm_sensor_listener_event_callback()) {
		dlog_print(DLOG_ERROR, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a HRM sensor listener.",
				__FILE__, __func__, __LINE__);
		return false;
	} else
		dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a HRM sensor listener.",
				__FILE__, __func__, __LINE__);

	//hrm_led_green_sensor
	int green_sensor_retval;
	green_sensor_retval = sensor_create_listener(hrm_led_green_sensor_handle,
			&hrm_led_green_sensor_listener_handle);

	if (green_sensor_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_create_listener() return value = %s",
				__FILE__, __func__, __LINE__,
				get_error_message(green_sensor_retval));
//		return false;
	}
	if (!set_hrm_led_green_sensor_listener_attribute()) {
		dlog_print(DLOG_ERROR, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to set an attribute to control the behavior of a HRM LED Green sensor listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in setting an attribute to control the behavior of a HRM LED Green sensor listener.",
				__FILE__, __func__, __LINE__);

	if (!set_hrm_led_green_sensor_listener_event_callback()) {
		dlog_print(DLOG_ERROR, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a HRM LED Green sensor listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a HRM LED Green sensor listener.",
				__FILE__, __func__, __LINE__);

	return true;
}

//attribute//
bool set_hrm_sensor_listener_attribute() {
	int retval;
	retval = sensor_listener_set_attribute_int(hrm_sensor_listener_handle,
			SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}

bool set_hrm_led_green_sensor_listener_attribute() {
	int retval;
	retval = sensor_listener_set_attribute_int(
			hrm_led_green_sensor_listener_handle, SENSOR_ATTRIBUTE_PAUSE_POLICY,
			SENSOR_PAUSE_NONE);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}

//event_callback//
bool set_hrm_sensor_listener_event_callback() {
	int retval;
	retval = sensor_listener_set_event_cb(hrm_sensor_listener_handle,
			hrm_sensor_listener_event_update_interval_ms,
			hrm_sensor_listener_event_callback, NULL);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}

bool set_hrm_led_green_sensor_listener_event_callback() {
	int retval;
	retval = sensor_listener_set_event_cb(hrm_led_green_sensor_listener_handle,
			hrm_sensor_listener_event_update_interval_ms,
			hrm_led_green_sensor_listener_event_callback, NULL);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}

/////////// Setting sensor listener event callback ///////////
void hrm_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], void *user_data) {
	int value = (int) events[0].values[0];
	dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG,
			"%s/%s/%d: Function sensor_events_callback() output value = %d",
			__FILE__, __func__, __LINE__, value);

	char * filepath = get_write_filepath("hda_sensor_data.txt");
	char msg_data[512];
	snprintf(msg_data, 512, "HRM output value = (%d)\n", value);
	append_file(filepath, msg_data);

	//	if(!set_gatt_characteristic_value(value))
	//		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "%s/%s/%d: Failed to update the value of a characteristic's GATT handle.", __FILE__, __func__, __LINE__);
	//	else
	//		dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "%s/%s/%d: Succeeded in updating the value of a characteristic's GATT handle.", __FILE__, __func__, __LINE__);
	//
	//	if(!notify_gatt_characteristic_value_changed())
	//		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "%s/%s/%d: Failed to notify value change of the characteristic to the remote devices which enable a Client Characteristic Configuration Descriptor.", __FILE__, __func__, __LINE__);
	//	else
	//		dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "%s/%s/%d: Succeeded in notifying value change of the characteristic to the remote devices which enable a Client Characteristic Configuration Descriptor.", __FILE__, __func__, __LINE__);
}

void hrm_led_green_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], void *user_data) {
	int value = (int) events[0].values[0];
	dlog_print(DLOG_INFO, HRM_LED_GREEN_SENSOR_LOG_TAG,
			"%s/%s/%d: HRM LED Green sensor_events_callback() output value = %d",
			__FILE__, __func__, __LINE__, value);

	char * filepath = get_write_filepath("hda_sensor_data.txt");
	char msg_data[512];
	snprintf(msg_data, 512, "HRM led green output value = (%d)\n", value);
	append_file(filepath, msg_data);
}

bool start_hrm_sensor_listener() {
	int retval;
	retval = sensor_listener_start(hrm_sensor_listener_handle);

	dlog_print(DLOG_INFO, HRM_SENSOR_LOG_TAG, "%i : %s : %i", retval,
			get_error_message(retval), hrm_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_start() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	int green_sensor_retval;
	green_sensor_retval = sensor_listener_start(
			hrm_led_green_sensor_listener_handle);

	dlog_print(DLOG_INFO, HRM_LED_GREEN_SENSOR_LOG_TAG, "%i : %s : %i",
			green_sensor_retval, get_error_message(green_sensor_retval),
			hrm_led_green_sensor_listener_handle);

	if (green_sensor_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_start() return value = %s",
				__FILE__, __func__, __LINE__,
				get_error_message(green_sensor_retval));
//		return false;
	}

	return true;
}

bool stop_hrm_sensor_listener() {
	int retval;
	retval = sensor_listener_stop(hrm_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_stop() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	int green_sensor_retval;
	green_sensor_retval = sensor_listener_stop(
			hrm_led_green_sensor_listener_handle);
	if (green_sensor_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_stop() return value = %s",
				__FILE__, __func__, __LINE__,
				get_error_message(green_sensor_retval));
//		return false;
	}
	return true;
}

bool destroy_hrm_sensor_listener() {
	int retval;
	retval = sensor_destroy_listener(hrm_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_destroy_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else {
		hrm_sensor_listener_handle = 0;
	}

	int green_sensor_retval;
	green_sensor_retval = sensor_destroy_listener(
			hrm_led_green_sensor_listener_handle);

	if (green_sensor_retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, HRM_LED_GREEN_SENSOR_LOG_TAG,
				"%s/%s/%d: HRM LED green sensor_destroy_listener() return value = %s",
				__FILE__, __func__, __LINE__,
				get_error_message(green_sensor_retval));
//		return false;
	} else {
		hrm_led_green_sensor_listener_handle = 0;
	}
	return true;
}

bool check_hrm_sensor_listener_is_created() {
	if (hrm_sensor_listener_handle == 0)
		return false;
	if (hrm_led_green_sensor_listener_handle == 0)
		return false;
	return true;
}

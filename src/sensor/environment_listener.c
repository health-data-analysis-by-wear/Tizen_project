/*
 * environment_listener.c
 *
 *  Created on: Feb 16, 2023
 *      Author: yuju
 */

#include <sensor/environment_listener.h>
#include "hda_service.h"
#include "bluetooth/gatt/characteristic.h"
#include <tools/sqlite_helper.h>

sensor_listener_h light_sensor_listener_handle = 0;
sensor_listener_h pedometer_listener_handle = 0;
sensor_listener_h pressure_sensor_listener_handle = 0;
sensor_listener_h sleep_monitor_listener_handle = 0;

unsigned int environment_sensor_listener_event_update_interval_ms = 1000;

char date_buf[64];

static void light_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], int events_count, void *user_data);
static void pedometer_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], int events_count, void *user_data);
static void pressure_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], int events_count, void *user_data);
static void sleep_monitor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], int events_count, void *user_data);

bool create_environment_sensor_listener(sensor_h light_sensor_handle,
		sensor_h pedometer_handle, sensor_h pressure_sensor_handle,
		sensor_h sleep_monitor_handle) {
	int retval;

	retval = sensor_create_listener(light_sensor_handle,
			&light_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_create_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	if (!set_light_sensor_listener_attribute()) {
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to set an attribute to control the behavior of a Light sensor listener.",
				__FILE__, __func__, __LINE__);
		return false;
	} else
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in setting an attribute to control the behavior of a Light sensor listener.",
				__FILE__, __func__, __LINE__);

	if (!set_light_sensor_listener_event_callback()) {
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a Light sensor listener.",
				__FILE__, __func__, __LINE__);
		return false;
	} else
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a Light sensor listener.",
				__FILE__, __func__, __LINE__);

	retval = sensor_create_listener(pedometer_handle,
			&pedometer_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Function sensor_create_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	if (!set_pedometer_listener_attribute()) {
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Failed to set an attribute to control the behavior of a Pedometer listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Succeeded in setting an attribute to control the behavior of a Pedometer listener.",
				__FILE__, __func__, __LINE__);

	if (!set_pedometer_listener_event_callback()) {
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a Pedometer listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a Pedometer listener.",
				__FILE__, __func__, __LINE__);

	retval = sensor_create_listener(pressure_sensor_handle,
			&pressure_sensor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_create_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
//		return false;
	}
	if (!set_pressure_sensor_listener_attribute()) {
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to set an attribute to control the behavior of a pressure sensor listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in setting an attribute to control the behavior of a pressure sensor listener.",
				__FILE__, __func__, __LINE__);

	if (!set_pressure_sensor_listener_event_callback()) {
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a pressure sensor listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a pressure sensor listener.",
				__FILE__, __func__, __LINE__);

	//sleep_monitor
	retval = sensor_create_listener(sleep_monitor_handle,
			&sleep_monitor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Function sensor_create_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
//		return false;
	}
	if (!set_sleep_monitor_listener_attribute()) {
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Failed to set an attribute to control the behavior of a sleep_monitor listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Succeeded in setting an attribute to control the behavior of a sleep_monitor listener.",
				__FILE__, __func__, __LINE__);

	if (!set_sleep_monitor_listener_event_callback()) {
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a sleep_monitor listener.",
				__FILE__, __func__, __LINE__);
//		return false;
	} else
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a sleep_monitor listener.",
				__FILE__, __func__, __LINE__);

	// launched time
	struct tm* t;
	time_t base = time(NULL);
	t = localtime(&base);
	snprintf(date_buf, 64, "%d-%d-%d %d:%d:%d", t->tm_year + 1900,
			t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	return true;
}

//attribute//
bool set_light_sensor_listener_attribute() {
	int retval;
	retval = sensor_listener_set_attribute_int(light_sensor_listener_handle,
			SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

bool set_pedometer_listener_attribute() {
	int retval;
	retval = sensor_listener_set_attribute_int(pedometer_listener_handle,
			SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

bool set_pressure_sensor_listener_attribute() {
	int retval;
	retval = sensor_listener_set_attribute_int(pressure_sensor_listener_handle,
			SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

//set_sleep_monitor_listener_attribute
bool set_sleep_monitor_listener_attribute() {
	int retval;
	retval = sensor_listener_set_attribute_int(sleep_monitor_listener_handle,
			SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

//event_callback//
bool set_light_sensor_listener_event_callback() {
	int retval;
	retval = sensor_listener_set_event_cb(light_sensor_listener_handle,
			environment_sensor_listener_event_update_interval_ms,
			light_sensor_listener_event_callback, NULL);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

bool set_pedometer_listener_event_callback() {
	int retval;
	retval = sensor_listener_set_event_cb(pedometer_listener_handle,
			environment_sensor_listener_event_update_interval_ms,
			pedometer_listener_event_callback, NULL);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

bool set_pressure_sensor_listener_event_callback() {
	int retval;
	retval = sensor_listener_set_event_cb(pressure_sensor_listener_handle,
			environment_sensor_listener_event_update_interval_ms,
			pressure_sensor_listener_event_callback, NULL);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

bool set_sleep_monitor_listener_event_callback() {
	int retval;
	retval = sensor_listener_set_event_cb(sleep_monitor_listener_handle,
			environment_sensor_listener_event_update_interval_ms,
			sleep_monitor_listener_event_callback, NULL);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else
		return true;
}

/////////// Setting sensor listener event callback ///////////
void light_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], int events_count, void *user_data) {
	dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
			"%s/%s/%d: Function sensor_events_callback() output value = (%s, %llu, %f)",
			__FILE__, __func__, __LINE__, date_buf, events[0].timestamp, events[0].values[0]);

	char * filepath = get_write_filepath("hda_sensor_data.txt");
	char msg_data[512];
	snprintf(msg_data, 512, "Light output value = (%s, %llu, %f)\n", date_buf, events[0].timestamp,
			events[0].values[0]);
	append_file(filepath, msg_data);

	for (int i = 0; i < events_count; i++) {
		float light_level = events[i].values[0];

		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_events_callback() output value = %f",
				__FILE__, __func__, __LINE__, light_level);
	}
}

void pedometer_listener_event_callback(sensor_h sensor, sensor_event_s events[],
		int events_count, void *user_data) {
	char * state;

	sensor_pedometer_state_e pedometer_state = events[0].values[7];

	if (pedometer_state == SENSOR_PEDOMETER_STATE_RUN) {
		state = "SENSOR_PEDOMETER_STATE_RUN";
	} else if (pedometer_state == SENSOR_PEDOMETER_STATE_STOP) {
		state = "SENSOR_PEDOMETER_STATE_STOP";
	} else if (pedometer_state == SENSOR_PEDOMETER_STATE_WALK) {
		state = "SENSOR_PEDOMETER_STATE_WALK";
	} else {
		state = "SENSOR_PEDOMETER_STATE_UNKNOWN";
	}
	//SENSOR_PEDOMETER_STATE_RUN 2 | SENSOR_PEDOMETER_STATE_STOP 0 | SENSOR_PEDOMETER_STATE_WALK 1 | SENSOR_PEDOMETER_STATE_WALK -1
	dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG, "state is %s (%d)", state,
			pedometer_state);
	dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
			"Pedometer event received: [%s/%s/%d], number_of_steps=%f, number_of_walking_steps=%f, number_of_running_steps=%f, moving_distance=%f, calories_burned=%f, last_speed=%f, last_stepping_frequency=%f, last_pedestrian_state=%s",
			__FILE__, __func__, __LINE__, events[0].values[0],
			events[0].values[1], events[0].values[2], events[0].values[3],
			events[0].values[4], events[0].values[5], events[0].values[6],
			state);

	char * filepath = get_write_filepath("hda_sensor_data.txt");
	char msg_data[512];
	snprintf(msg_data, 512,
			"Pedometer output value = (%s, %llu, %f, %f, %f, %f, %f, %f, %f, %s)\n",
			date_buf, events[0].timestamp, events[0].values[0], events[0].values[1],
			events[0].values[2], events[0].values[3], events[0].values[4],
			events[0].values[5], events[0].values[6], state);
	append_file(filepath, msg_data);

	for (int i = 0; i < events_count; i++) {
		float number_of_steps = events[i].values[0];
		float number_of_walking_steps = events[i].values[1];
		float number_of_running_steps = events[i].values[2];
		float moving_distance = events[i].values[3];
		float calories_burned = events[i].values[4];
		float last_speed = events[i].values[5];
		float last_stepping_frequency = events[i].values[6];

		if (events[i].values[7] == SENSOR_PEDOMETER_STATE_RUN) {
			state = "SENSOR_PEDOMETER_STATE_RUN";
		} else if (events[i].values[7] == SENSOR_PEDOMETER_STATE_STOP) {
			state = "SENSOR_PEDOMETER_STATE_STOP";
		} else if (events[i].values[7] == SENSOR_PEDOMETER_STATE_WALK) {
			state = "SENSOR_PEDOMETER_STATE_WALK";
		} else {
			state = "SENSOR_PEDOMETER_STATE_UNKNOWN";
		}

		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"Pedometer event received: [%s/%s/%d], number_of_steps=%f, number_of_walking_steps=%f, number_of_running_steps=%f, moving_distance=%f, calories_burned=%f, last_speed=%f, last_stepping_frequency=%f, last_pedestrian_state=%f",
				__FILE__, __func__, __LINE__, number_of_steps,
				number_of_walking_steps, number_of_running_steps,
				moving_distance, calories_burned, last_speed,
				last_stepping_frequency, state);
	}
}

void pressure_sensor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], int events_count, void *user_data) {
	dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
			"%s/%s/%d: Function sensor_events_callback() output value = (%s, %llu, %f)",
			__FILE__, __func__, __LINE__, date_buf, events[0].timestamp, events[0].values[0]);

	char * filepath = get_write_filepath("hda_sensor_data.txt");
	char msg_data[512];
	snprintf(msg_data, 512, "Pressure output value = (%s, %llu, %f)\n", date_buf, events[0].timestamp,
			events[0].values[0]);
	append_file(filepath, msg_data);

	for (int i = 0; i < events_count; i++) {
		//int accuracy = events[i].accuracy;
		float pressure_level = events[i].values[0];

		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_events_callback() output value = %f",
				__FILE__, __func__, __LINE__, pressure_level);
	}
}

void sleep_monitor_listener_event_callback(sensor_h sensor,
		sensor_event_s events[], int events_count, void *user_data) {
	char * state;
	sensor_sleep_state_e sleep_state = events[0].values[0];
	if (sleep_state == SENSOR_SLEEP_STATE_WAKE) {
		state = "SENSOR_SLEEP_STATE_WAKE";
	} else if (sleep_state == SENSOR_SLEEP_STATE_SLEEP) {
		state = "SENSOR_SLEEP_STATE_SLEEP";
	} else {
		state = "SENSOR_SLEEP_STATE_UNKNOWN";
	}
	dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
			"%s/%s/%d: Function sensor_events_callback() output value = (%s, %llu, %s)",
			__FILE__, __func__, __LINE__, date_buf, events[0].timestamp, state);

	char * filepath = get_write_filepath("hda_sensor_data.txt");
	char msg_data[512];
	snprintf(msg_data, 512, "Sleep monitor output value = (%s, %llu, %s)\n", date_buf, events[0].timestamp,
			state);
	append_file(filepath, msg_data);

	for (int i = 0; i < events_count; i++) {
		int accuracy = events[i].accuracy;
		if (events[i].values[0] == SENSOR_SLEEP_STATE_WAKE) {
			state = "SENSOR_SLEEP_STATE_WAKE";
		} else if (events[i].values[0] == SENSOR_SLEEP_STATE_SLEEP) {
			state = "SENSOR_SLEEP_STATE_SLEEP";
		} else {
			state = "SENSOR_SLEEP_STATE_UNKNOWN";
		}

		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Sleep monitor event received: accuracy=%d, state=%d",
				__FILE__, __func__, __LINE__, accuracy, state);
	}
}

bool start_environment_sensor_listener() {
	int retval;
	retval = sensor_listener_start(light_sensor_listener_handle);
	dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG, "%i : %s : %i", retval,
			get_error_message(retval), light_sensor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_start() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	retval = sensor_listener_start(pedometer_listener_handle);
	dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG, "%i : %s : %i", retval,
			get_error_message(retval), pedometer_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_start() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	retval = sensor_listener_start(pressure_sensor_listener_handle);
	dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG, "%i : %s : %i", retval,
			get_error_message(retval), pressure_sensor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_start() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	retval = sensor_listener_start(sleep_monitor_listener_handle);
	dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG, "%i : %s : %i", retval,
			get_error_message(retval), sleep_monitor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_ERROR, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_start() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	return true;

}

bool stop_environment_sensor_listener() {
	int retval;
	retval = sensor_listener_stop(light_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_stop() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	retval = sensor_listener_stop(pedometer_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_stop() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	retval = sensor_listener_stop(pressure_sensor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_stop() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	retval = sensor_listener_stop(sleep_monitor_listener_handle);
	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Function sensor_listener_stop() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	return true;
}

bool destroy_environment_sensor_listener() {
	int retval;
	retval = sensor_destroy_listener(light_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, LIGHT_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_destroy_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else {
		light_sensor_listener_handle = 0;
	}

	retval = sensor_destroy_listener(pedometer_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PEDOMETER_LOG_TAG,
				"%s/%s/%d: Function sensor_destroy_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else {
		pedometer_listener_handle = 0;
	}

	retval = sensor_destroy_listener(pressure_sensor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, PRESSURE_SENSOR_LOG_TAG,
				"%s/%s/%d: Function sensor_destroy_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else {
		pressure_sensor_listener_handle = 0;
	}

	retval = sensor_destroy_listener(sleep_monitor_listener_handle);

	if (retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_INFO, SLEEP_MONITOR_LOG_TAG,
				"%s/%s/%d: Function sensor_destroy_listener() return value = %s",
				__FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	} else {
		sleep_monitor_listener_handle = 0;
	}
	return true;
}

bool check_environment_sensor_listener_is_created() {
	if (light_sensor_listener_handle == 0)
		return false;
	if (pedometer_listener_handle == 0)
		return false;
	if (pressure_sensor_listener_handle == 0)
		return false;
	if (sleep_monitor_listener_handle == 0)
		return false;
	else
		return true;

}

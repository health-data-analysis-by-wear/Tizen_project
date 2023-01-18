#include <sensor/physics_listener.h>
#include "hda_service.h"


sensor_listener_h accelerometer_sensor_listener_handle = 0;
sensor_listener_h gravity_sensor_listener_handle = 0;
unsigned int physics_sensor_listener_event_update_interval_ms = 200;

static void accelerometer_sensor_listener_event_callback(sensor_h sensor, sensor_event_s events[], int events_count, void *user_data);
static void gravity_sensor_listener_event_callback(sensor_h sensor, sensor_event_s events[], int events_count, void *user_data);

bool create_physics_sensor_listener(sensor_h accelerometer_sensor_handle, sensor_h gravity_sensor_handle) {
	int retval;

	retval = sensor_create_listener(accelerometer_sensor_handle, &accelerometer_sensor_listener_handle);
	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, PHYSICS_SENSOR_LOG_TAG, "%s/%s/%d: Accelerometer sensor_create_listener() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	if(!set_accelerometer_sensor_listener_attribute())
	{
		dlog_print(DLOG_ERROR, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Failed to set an attribute to control the behavior of a Accelerometer sensor listener.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Succeeded in setting an attribute to control the behavior of a Accelerometer sensor listener.", __FILE__, __func__, __LINE__);

	if(!set_accelerometer_sensor_listener_event_callback())
	{
		dlog_print(DLOG_ERROR, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a Accelerometer sensor listener.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a Accelerometer sensor listener.", __FILE__, __func__, __LINE__);


	retval = sensor_create_listener(gravity_sensor_handle, &gravity_sensor_listener_handle);
	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Gravity sensor_create_listener() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
//		return false;
	}

	if(!set_gravity_sensor_listener_attribute())
	{
		dlog_print(DLOG_ERROR, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Failed to set an attribute to control the behavior of a Gravity sensor listener.", __FILE__, __func__, __LINE__);
//		return false;
	}
	else
		dlog_print(DLOG_INFO, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Succeeded in setting an attribute to control the behavior of a Gravity sensor listener.", __FILE__, __func__, __LINE__);

	if(!set_gravity_sensor_listener_event_callback())
	{
		dlog_print(DLOG_ERROR, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a Gravity sensor listener.", __FILE__, __func__, __LINE__);
//		return false;
	}
	else
		dlog_print(DLOG_INFO, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a Gravity sensor listener.", __FILE__, __func__, __LINE__);

	return true;
}


/////////// Setting sensor listener attribue ///////////

bool set_accelerometer_sensor_listener_attribute()
{
	int retval;
	retval = sensor_listener_set_attribute_int(accelerometer_sensor_listener_handle, SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_gravity_sensor_listener_attribute()
{
	int retval;
	retval = sensor_listener_set_attribute_int(gravity_sensor_listener_handle, SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}


/////////// Setting sensor listener event callback ///////////

bool set_accelerometer_sensor_listener_event_callback()
{
	int retval;
	retval = sensor_listener_set_event_cb(accelerometer_sensor_listener_handle, physics_sensor_listener_event_update_interval_ms, accelerometer_sensor_listener_event_callback, NULL);

	if(retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_gravity_sensor_listener_event_callback()
{
	int retval;
	retval = sensor_listener_set_event_cb(gravity_sensor_listener_handle, physics_sensor_listener_event_update_interval_ms, gravity_sensor_listener_event_callback, NULL);

	if(retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}


/////////// Setting sensor listener event callback ///////////

void accelerometer_sensor_listener_event_callback(sensor_h sensor, sensor_event_s events[], int events_count, void *user_data) {
	dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_events_callback() output value = (%i, %i, %f, %f, %f)", __FILE__, __func__, __LINE__, events[0].timestamp, events[0].accuracy, events[0].values[0], events[0].values[1], events[0].values[2]);
	for(int i = 1; i < events_count; i++){
		unsigned long long timestamp = events[i].timestamp;
		int accuracy = events[i].accuracy;
		float x = events[i].values[0];
		float y = events[i].values[1];
		float z = events[i].values[2];

		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_events_callback() output value = (%f, %f, %f)", __FILE__, __func__, __LINE__, x, y, z);
	}
}

void gravity_sensor_listener_event_callback(sensor_h sensor, sensor_event_s events[], int events_count, void *user_data) {
	dlog_print(DLOG_INFO, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_events_callback() output value = (%i, %f, %f, %f)", __FILE__, __func__, __LINE__, events[0].accuracy, events[0].values[0], events[0].values[1], events[0].values[2]);
	for(int i = 1; i < events_count; i++){
		unsigned long long timestamp = events[i].timestamp;
		int accuracy = events[i].accuracy;
		float x = events[i].values[0];
		float y = events[i].values[1];
		float z = events[i].values[2];

		dlog_print(DLOG_INFO, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_events_callback() output value = (%f, %f, %f)", __FILE__, __func__, __LINE__, x, y, z);
	}
}


/////////// Setting sensor listener event callback ///////////

bool start_physics_sensor_listener()
{
	int accelerometer_retval;
	accelerometer_retval = sensor_listener_start(accelerometer_sensor_listener_handle);
	if (accelerometer_retval == 0)
		dlog_print(DLOG_INFO, ACCELEROMETER_SENSOR_LOG_TAG, "%i : %s : %i", accelerometer_retval, get_error_message(accelerometer_retval), accelerometer_sensor_listener_handle);
	else
		dlog_print(DLOG_WARN, ACCELEROMETER_SENSOR_LOG_TAG, "%i : %s : %i", accelerometer_retval, get_error_message(accelerometer_retval), accelerometer_sensor_listener_handle);

	if(accelerometer_retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_start() return value = %s", __FILE__, __func__, __LINE__, get_error_message(accelerometer_retval));
		return false;
	}

	int gravity_retval;
	gravity_retval = sensor_listener_start(gravity_sensor_listener_handle);
	if (gravity_retval == 0)
		dlog_print(DLOG_INFO, GRAVITY_SENSOR_LOG_TAG, "%i : %s : %i", gravity_retval, get_error_message(gravity_retval), gravity_sensor_listener_handle);
	else
		dlog_print(DLOG_WARN, GRAVITY_SENSOR_LOG_TAG, "%i : %s : %i", gravity_retval, get_error_message(gravity_retval), gravity_sensor_listener_handle);

	if(gravity_retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_start() return value = %s", __FILE__, __func__, __LINE__, get_error_message(gravity_retval));
//		return false;
	}

	return true;
}

bool stop_physics_sensor_listener()
{
	int accelerometer_retval;
	accelerometer_retval = sensor_listener_stop(accelerometer_sensor_listener_handle);
	if(accelerometer_retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_stop() return value = %s", __FILE__, __func__, __LINE__, get_error_message(accelerometer_retval));
		return false;
	}

	int gravity_retval;
	gravity_retval = sensor_listener_stop(gravity_sensor_listener_handle);
	if(gravity_retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_listener_stop() return value = %s", __FILE__, __func__, __LINE__, get_error_message(gravity_retval));
//		return false;
	}

	return true;
}

bool destroy_physics_sensor_listener()
{
	int accelerometer_retval;
	accelerometer_retval = sensor_destroy_listener(accelerometer_sensor_listener_handle);

	if(accelerometer_retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, ACCELEROMETER_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_destroy_listener() return value = %s", __FILE__, __func__, __LINE__, get_error_message(accelerometer_retval));
		return false;
	}
	else
	{
		accelerometer_sensor_listener_handle = 0;
	}

	int gravity_retval;
	gravity_retval = sensor_destroy_listener(accelerometer_sensor_listener_handle);

	if(gravity_retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, GRAVITY_SENSOR_LOG_TAG, "%s/%s/%d: Function sensor_destroy_listener() return value = %s", __FILE__, __func__, __LINE__, get_error_message(gravity_retval));
//		return false;
	}
	else
	{
		gravity_sensor_listener_handle = 0;
	}

	return true;
}

bool check_physics_sensor_listener_is_created()
{
	if (accelerometer_sensor_listener_handle == 0)
		return false;
	if (gravity_sensor_listener_handle == 0)
//		return false;
	return true;
}

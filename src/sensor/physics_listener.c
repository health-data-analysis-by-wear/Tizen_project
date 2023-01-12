#include <sensor/physics_listener.h>
#include "hda_service.h"

static bool accelerometer_supported = false;
static bool gravity_supported = false;

static sensor_h accelerometer_sensor;
static sensor_h gravity_sensor;

static void physics_sensor_callback(sensor_h sensor, sensor_event_s events[], int events_count, void *user_data);

sensor_listener_h accelerometer_listener = 0;
sensor_listener_h gravity_listener = 0;

bool create_physics_sensor_listener(){
	sensor_is_supported(SENSOR_ACCELEROMETER, &accelerometer_supported);
	if (!accelerometer_supported) {
	    /* Accelerometer is not supported on the current device */
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "ACCELEROMETER is not supported on the current device");
	}
	else{
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Test: %i", accelerometer_listener);
		int sensor_get = sensor_get_default_sensor(SENSOR_ACCELEROMETER, accelerometer_sensor);
		int sensor_create = sensor_create_listener(accelerometer_sensor, &accelerometer_listener);
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Test: %i", accelerometer_listener);
		int set_attribute = sensor_listener_set_attribute_int(accelerometer_listener, SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Test: %i", accelerometer_listener);
		int set_event = sensor_listener_set_event_cb(accelerometer_listener, 1000, physics_sensor_callback, NULL);
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Test: %i", accelerometer_listener);

		if(!sensor_get){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "ACCELEROMETER: Failed to sensor get default sensor.");
		}
		else if(!sensor_create){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "ACCELEROMETER: Failed to sensor create.");
		}
		else if(!set_attribute){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "ACCELEROMETER: Failed to set attribute.");
		}
		else if(!set_event){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "ACCELEROMETER: Failed to set event.");
		}
		else{
			dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "ACCELEROMETER: Connected");
		}
	}

	sensor_is_supported(SENSOR_GRAVITY, &gravity_supported);
	if (!gravity_supported) {
	    /* Accelerometer is not supported on the current device */
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "GRAVITY is not supported on the current device");
	}
	else{
		int sensor_get = sensor_get_default_sensor(SENSOR_GRAVITY, gravity_sensor);
		int sensor_create = sensor_create_listener(gravity_sensor, &gravity_listener);
		int set_attribute = sensor_listener_set_attribute_int(gravity_listener, SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);
		int set_event = sensor_listener_set_event_cb(gravity_listener, 1000, physics_sensor_callback, NULL);

		if(sensor_get != SENSOR_ERROR_NONE){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "GRAVITY: Failed to sensor get default sensor.");
		}
		else if(sensor_create != SENSOR_ERROR_NONE){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "GRAVITY: Failed to sensor create.");
		}
		else if(set_attribute != SENSOR_ERROR_NONE){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "GRAVITY: Failed to set attribute.");
		}
		else if(set_event != SENSOR_ERROR_NONE){
			dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "GRAVITY: Failed to set event.");
		}
		else{
			dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "GRAVITY: Connected");
		}
	}
}

void physics_sensor_callback(sensor_h sensor, sensor_event_s events[], int events_count, void *user_data)
{
	dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Physics_sensor_callback");
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    int i = 0;

    if (type == SENSOR_ACCELEROMETER) {
       for (i = 0; i < events_count; i++) {
          unsigned long long timestamp = events[i].timestamp;
          int accuracy = events[i].accuracy;
          float x = events[i].values[0];
          float y = events[i].values[1];
          float z = events[i].values[2];

          dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "ACCELEROMETER: %f, %f, %f", x, y, z);
       }
    } else if (type == SENSOR_GRAVITY) {
    	for (i = 0; i < events_count; i++) {
    		unsigned long long timestamp = events[i].timestamp;
    	    int accuracy = events[i].accuracy;
    	    float x = events[i].values[0];
    	    float y = events[i].values[1];
    	    float z = events[i].values[2];

    	    dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "GRAVITY: %f, %f, %f", x, y, z);
    	}
    }
}

void sensor_launch_all(){
	int accel;
	int gravity;
	accel = sensor_listener_start(accelerometer_listener);
	gravity = sensor_listener_start(gravity_listener);

	if(accel != SENSOR_ERROR_NONE){
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Accel sensor is return value = %s : %i", get_error_message(accel), accel);
	}
	else if(gravity != SENSOR_ERROR_NONE){
		dlog_print(DLOG_ERROR, SENSOR_LOG_TAG, "Gravity sensor is return value = %s : %i", get_error_message(gravity), gravity);
	}
	else{
		dlog_print(DLOG_INFO, SENSOR_LOG_TAG, "Physics sensors launched");
	}
}

void sensor_stop_all(){
	sensor_listener_stop(accelerometer_listener);
	sensor_listener_stop(gravity_listener);
}

void sensor_destroy_all(){
	sensor_destroy_listener(accelerometer_listener);
	sensor_destroy_listener(gravity_listener);
}



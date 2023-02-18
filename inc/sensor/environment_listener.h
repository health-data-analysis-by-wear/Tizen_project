/*
 * environment_listener.h
 *
 *  Created on: Feb 16, 2023
 *      Author: yuju
 */

#ifndef SENSOR_ENVIRONMENT_LISTENER_H_
#define SENSOR_ENVIRONMENT_LISTENER_H_

#include <hda_service.h>

bool create_environment_sensor_listener(sensor_h light_sensor_handle, sensor_h pedometer_handle, sensor_h pressure_sensor_handle, sensor_h sleep_monitor_handle);

bool set_light_sensor_listener_attribute();
bool set_light_sensor_listener_event_callback();
bool set_pedometer_listener_attribute();
bool set_pedometer_listener_event_callback();
bool set_pressure_sensor_listener_attribute();
bool set_pressure_sensor_listener_event_callback();
bool set_sleep_monitor_listener_attribute();
bool set_sleep_monitor_listener_event_callback();

bool start_environment_sensor_listener();
bool stop_environment_sensor_listener();
bool destroy_environment_sensor_listener();
bool check_environment_sensor_listener_is_created();


#endif /* SENSOR_ENVIRONMENT_LISTENER_H_ */

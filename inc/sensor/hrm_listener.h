#ifndef SENSOR_HRM_LISTENER_H_
#define SENSOR_HRM_LISTENER_H_

#include <hda_service.h>

bool create_hrm_sensor_listener(sensor_h hrm_sensor_handle, sensor_h hrm_led_green_sensor_handle);

bool set_hrm_sensor_listener_attribute();
bool set_hrm_sensor_listener_event_callback();
bool set_hrm_led_green_sensor_listener_attribute();
bool set_hrm_led_green_sensor_listener_event_callback();

bool start_hrm_sensor_listener();
bool stop_hrm_sensor_listener();
bool destroy_hrm_sensor_listener();
bool check_hrm_sensor_listener_is_created();

#endif /* SENSOR_HRM_LISTENER_H_ */

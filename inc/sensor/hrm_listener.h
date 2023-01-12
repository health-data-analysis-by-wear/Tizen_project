#ifndef SENSOR_HRM_LISTENER_H_
#define SENSOR_HRM_LISTENER_H_

#include <hda_service.h>

bool create_hrm_sensor_listener(sensor_h sensor);
bool set_hrm_sensor_listener_attribute();
bool set_hrm_sensor_listener_event_callback();
bool start_hrm_sensor_listener();
bool stop_hrm_sensor_listener();
bool destroy_hrm_sensor_listener();
bool check_hrm_sensor_listener_is_created();

bool create_physics_sensor_listener();
void physics_sensor_callback(sensor_h sensor, sensor_event_s events[], int events_count, void *user_data);
void sensor_launch_all();
void sensor_stop_all();
void sensor_destroy_all();

#endif /* SENSOR_HRM_LISTENER_H_ */

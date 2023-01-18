#ifndef SENSOR_PHYSICS_LISTENER_H_
#define SENSOR_PHYSICS_LISTENER_H_

#include <hda_service.h>

bool create_physics_sensor_listener(sensor_h accelerometer_sensor_handle, sensor_h gravity_sensor_handle);
bool set_accelerometer_sensor_listener_attribute();
bool set_gravity_sensor_listener_attribute();
bool set_accelerometer_sensor_listener_event_callback();
bool set_gravity_sensor_listener_event_callback();
bool start_physics_sensor_listener();
bool stop_physics_sensor_listener();
bool destroy_physics_sensor_listener();
bool check_physics_sensor_listener_is_created();


#endif /* SENSOR_PHYSICS_H_ */

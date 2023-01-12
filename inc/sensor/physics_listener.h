#ifndef SENSOR_PHYSICS_LISTENER_H_
#define SENSOR_PHYSICS_LISTENER_H_

#include <hda_service.h>


bool create_physics_sensor_listener();
void sensor_launch_all();
void sensor_stop_all();
void sensor_destroy_all();

#endif /* SENSOR_PHYSICS_LISTENER_H_ */

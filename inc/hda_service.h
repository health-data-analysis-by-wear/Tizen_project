#ifndef __hda_service_H__
#define __hda_service_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include <sensor.h>
#include <bluetooth.h>
#include <time.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "hda_service"
#define HRM_SENSOR_LOG_TAG "HRM_SENSOR_EVENT"
#define HRM_LED_GREEN_SENSOR_LOG_TAG "HRM_LED_GREEN_SENSOR_EVENT"

#define PHYSICS_SENSOR_LOG_TAG "PHYSICS_SENSOR_EVENT"
#define ACCELEROMETER_SENSOR_LOG_TAG "ACCELEROMETE_SENSOR_EVENT"
#define GRAVITY_SENSOR_LOG_TAG "GRAVITY_SENSOR_EVENT"
#define BLUETOOTH_LOG_TAG "BLUETOOTH_EVENT"
#define GYROSCOPE_ROTATION_VECTOR_SENSOR_LOG_TAG "GYROSCOPE_ROTATION_VECTOR_SENSOR_EVENT"
#define GYROSCOPE_SENSOR_LOG_TAG "GYROSCOPE_SENSOR_EVENT"
#define LINEAR_ACCELERATION_SENSOR_LOG_TAG "LINEAR_ACCLERATION_SENSOR_EVENT"

#define ENVIRONMENT_SENSOR_LOG_TAG "ENVIRONMENT_SENSOR_EVENT"
#define LIGHT_SENSOR_LOG_TAG "LIGHT_SENSOR_EVENT"
#define PEDOMETER_LOG_TAG "PEDOMETER_EVENT" //
#define PRESSURE_SENSOR_LOG_TAG "PRESSURE_SENSOR_EVENT"
#define SLEEP_MONITOR_LOG_TAG "SLEEP_MONITOR_EVENT" //

#define SQLITE3_LOG_TAG "SQLITE3_EVENT"

#if !defined(PACKAGE)
#define PACKAGE "org.example.hda_service"
#endif

#endif /* __hda_service_H__ */

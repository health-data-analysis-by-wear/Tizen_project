#ifndef __hda_service_H__
#define __hda_service_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include <sensor.h>
#include <bluetooth.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "hda_service"
#define SENSOR_LOG_TAG "SENSOR_EVENT"
#define BLUETOOTH_LOG_TAG "BLUETOOTH_EVENT"

#if !defined(PACKAGE)
#define PACKAGE "org.example.hda_service"
#endif

#endif /* __hda_service_H__ */

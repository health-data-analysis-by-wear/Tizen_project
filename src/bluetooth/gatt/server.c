#include <sensor/hrm_listener.h>
#include "hda_service.h"
#include "bluetooth/gatt/server.h"
#include "bluetooth/gatt/service.h"
#include "bluetooth/le/advertiser.h"

static bt_gatt_server_h gatt_server_handle = 0;

void gatt_server_connection_state_changed_callback(int result, bool connected, const char *remote_address, void *user_data);

bool create_gatt_server()
{
	int retval;

	retval = bt_gatt_server_create(&gatt_server_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "%s/%s/%d: Function bt_gatt_server_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	if(!set_gett_server_connection_state_changed_callback())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "%s/%s/%d: Failed to register a callback function that will be invoked when the GATT server connection state is changed.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "%s/%s/%d: Succeeded in registering a callback function that will be invoked when the GATT server connection state is changed.", __FILE__, __func__, __LINE__);

	if(!register_gatt_service_to_gatt_server())
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "%s/%s/%d: Failed to register a specified service to the specified GATT server that the local device is hosting.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
	{
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "%s/%s/%d: Succeeded in registering a specified service to the specified GATT server that the local device is hosting.", __FILE__, __func__, __LINE__);
		return true;
	}
}

bool set_gett_server_connection_state_changed_callback()
{
	int retval;

	retval = bt_gatt_set_connection_state_changed_cb(gatt_server_connection_state_changed_callback, NULL);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "%s/%s/%d: Function bt_gatt_set_connection_state_changed_cb() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

void gatt_server_connection_state_changed_callback(int result, bool connected, const char *remote_address, void *user_data)
{
	if(!connected)
	{
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "%s/%s/%d: A GATT server is disconnected.", __FILE__, __func__, __LINE__);
//		ui_app_exit();
	}
	else
	{
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "%s/%s/%d: A GATT server is connected.", __FILE__, __func__, __LINE__);

		if(!stop_bluetooth_le_advertising())
			dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "%s/%s/%d: Failed to stop the advertising.", __FILE__, __func__, __LINE__);
		else
			dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "%s/%s/%d: Succeeded in stopping the advertising.", __FILE__, __func__, __LINE__);
	}
}

bool register_gatt_service_to_gatt_server()
{
	int retval;
	bt_gatt_h gatt_service_handle = 0;

	if (!get_gatt_service_handle(&gatt_service_handle))
	{
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "%s/%s/%d: Failed to get the GATT service's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "%s/%s/%d: Succeeded in getting the GATT service's handle.", __FILE__, __func__, __LINE__);

	retval = bt_gatt_server_register_service(gatt_server_handle, gatt_service_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "%s/%s/%d: Function bt_gatt_server_register_service() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool start_gatt_server()
{
	int retval;

	retval = bt_gatt_server_start();

	if(retval != BT_ERROR_NONE)
		{
			dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "%s/%s/%d: Function bt_gatt_server_start() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
			return false;
		}
		else
			return true;
}

bool destroy_gatt_server()
{
	int retval;

	retval = bt_gatt_server_destroy(gatt_server_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, BLUETOOTH_LOG_TAG, "%s/%s/%d: Function bt_gatt_server_destroy() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
	{
		gatt_server_handle = 0;
		return true;
	}
}

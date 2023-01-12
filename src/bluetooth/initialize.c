#include <dlog.h>
#include "hda_service.h"
#include "bluetooth/initialize.h"

#include <app_control.h>
#include <glib.h>

bool bluetooth_initialize(){
	bt_error_e ret;
	ret = bt_initialize();

	if(ret != BT_ERROR_NONE){
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "[bt_initialize] failed.");
		return false;
	}
	return true;
}



// Not working in watch3
int bluetooth_operate(void){
	int ret = 0;
	app_control_h service = NULL;
	app_control_create(&service);
	if(service == NULL){
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "service_create failed\n");
		return 0;
	}
	app_control_set_operation(service, APP_CONTROL_OPERATION_SETTING_BT_ENABLE);
	ret = app_control_send_launch_request(service, NULL, NULL);

	app_control_destroy(service);
	if (ret == APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Succeeded to Bluetooth On/Off app!\n");
		return 0;
	} else {
		dlog_print(DLOG_INFO, BLUETOOTH_LOG_TAG, "Failed to relaunch Bluetooth On/Off app!\n");
		return -1;
	}

	    return 0;
}

bool bluetooth_check_adapter(){
	bt_adapter_state_e adapter_state;

	int ret = bt_adapter_get_state(&adapter_state);
	if(ret != BT_ERROR_NONE){
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "[bt_adapter_get_state] failed");
		return false;
	}
	if(adapter_state == BT_ADAPTER_DISABLED){
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "Bluetooth adapter is not enabled. you should enable bluetooth.");
		return false;
	}


	bt_error_e bt_error_ret;
	bt_error_ret = bt_adapter_set_state_changed_cb(adapter_state_changed_cb, NULL);
	if(bt_error_ret != BT_ERROR_NONE){
		dlog_print(DLOG_ERROR, BLUETOOTH_LOG_TAG, "[bt_adapter_set_state_changed_cb()] failed.");
		return false;
	}

	return true;
}

void adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state, void* user_data)
{
    if (result != BT_ERROR_NONE) {
        dlog_print(DLOG_ERROR, LOG_TAG, "[adapter_state_changed_cb] failed! result=%d", result);

        return;
    }
    if (adapter_state == BT_ADAPTER_ENABLED) {
        dlog_print(DLOG_INFO, LOG_TAG, "[adapter_state_changed_cb] Bluetooth is enabled!");

        char *local_address = NULL;
        bt_adapter_get_address(&local_address);
        dlog_print(DLOG_INFO, LOG_TAG, "[adapter_state_changed_cb] Adapter address: %s.", local_address);
        if (local_address)
            free(local_address);
        char *local_name;
        bt_adapter_get_name(&local_name);
        dlog_print(DLOG_INFO, LOG_TAG, "[adapter_state_changed_cb] Adapter name: %s.", local_name);
        if (local_name)
            free(local_name);


        bt_adapter_visibility_mode_e mode;
        int duration = 1;

        bt_adapter_get_visibility(&mode, &duration);
        switch (mode) {
        case BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE:
            dlog_print(DLOG_INFO, LOG_TAG,
                       "[adapter_state_changed_cb] Visibility: NON_DISCOVERABLE");
            break;
        case BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE:
            dlog_print(DLOG_INFO, LOG_TAG,
                       "[adapter_state_changed_cb] Visibility: GENERAL_DISCOVERABLE");
            break;
        case BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE:
            dlog_print(DLOG_INFO, LOG_TAG,
                       "[adapter_state_changed_cb] Visibility: LIMITED_DISCOVERABLE");
            break;
        }
    } else {
        dlog_print(DLOG_INFO, LOG_TAG, "[adapter_state_changed_cb] Bluetooth is disabled!");
    }
}




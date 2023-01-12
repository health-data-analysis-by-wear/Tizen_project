#ifndef BLUETOOTH_INITIALIZE_H_
#define BLUETOOTH_INITIALIZE_H_


bool bluetooth_initialize();
int bluetooth_operate();
bool bluetooth_check_adapter();
void adapter_state_changed_cb();


#endif /* BLUETOOTH_INITIALIZE_H_ */

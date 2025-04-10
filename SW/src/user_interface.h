#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

void setup_led();
void loop_led();

void setup_serial(const __FlashStringHelper* helloStr);
void loop_serial(bool *pBusy, size_t num_servers);

void end_of_setup();

#endif
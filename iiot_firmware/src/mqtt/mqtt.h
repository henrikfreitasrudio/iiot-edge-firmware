#pragma once

#include <stdbool.h>

void mqtt_start(void);
bool mqtt_is_connected(void);
void mqtt_publish(const char *topic, const char *msg);
#include "sensors.h"
#include "esp_random.h"

void sensors_get_data(float *temp, float *pressure)
{
    *temp = 20 + (esp_random() % 100) / 10.0;
    *pressure = 60 + (esp_random() % 100) / 10.0;
}
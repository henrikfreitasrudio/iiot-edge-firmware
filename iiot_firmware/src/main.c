#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi/wifi.h"
#include "mqtt/mqtt.h"
#include "sensors/sensors.h"

static const char *TAG = "APP";

void send_data_task(void *pvParameters)
{
    while (1)
    {
        if (mqtt_is_connected())
        {
            float temp, pressure;
            sensors_get_data(&temp, &pressure);

            char msg[128];

            snprintf(msg, sizeof(msg),
                     "{\"device\":\"esp32_c6\",\"temperature\":%.2f,\"pressure\":%.2f}",
                     temp, pressure);

            mqtt_publish("iot/sensors", msg);

            ESP_LOGI(TAG, "Enviado: %s", msg);
        }
        else
        {
            ESP_LOGW(TAG, "MQTT não conectado...");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void)
{
    nvs_flash_init();

    wifi_init();

    xTaskCreate(send_data_task, "send_data", 4096, NULL, 5, NULL);
}
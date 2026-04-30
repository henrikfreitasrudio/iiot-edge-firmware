#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_random.h"
#include "esp_netif.h"

#define WIFI_SSID "Freitas_VillaggioNet"
#define WIFI_PASS "agewarcraft"

static const char *TAG = "IIOT";

static esp_mqtt_client_handle_t client;

void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    if (netif)
    {
        esp_netif_get_ip_info(netif, &ip_info);
        ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info.ip));
    }

    ESP_LOGI(TAG, "WiFi iniciado...");
}

void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://10.0.0.100:1883", // IP da Raspberry
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "MQTT iniciado...");
    ESP_LOGI(TAG, "Conectando no MQTT: mqtt://10.0.0.100:1883");
}

void send_data_task(void *pvParameters)
{
    while (1)
    {
        float temp = 20 + (esp_random() % 100) / 10.0;
        float pressure = 60 + (esp_random() % 100) / 10.0;

        char msg[128];

        snprintf(msg, sizeof(msg),
                 "{\"device\":\"esp32_c6\",\"temperature\":%.2f,\"pressure\":%.2f}",
                 temp, pressure);

        esp_mqtt_client_publish(client, "iot/sensors", msg, 0, 1, 0);

        ESP_LOGI(TAG, "Enviado: %s", msg);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void)
{
    nvs_flash_init();

    wifi_init();
    vTaskDelay(pdMS_TO_TICKS(5000)); // espera conectar

    mqtt_start();

    xTaskCreate(send_data_task, "send_data", 4096, NULL, 5, NULL);
}
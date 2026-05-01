#include <stdio.h>
#include <stdbool.h>

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
static bool mqtt_connected = false;

/* ================= PROTOTYPE ================= */
void mqtt_start(void);

/* ================= WIFI EVENT ================= */

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Conectando ao WiFi...");
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG, "WiFi desconectado, reconectando...");
        esp_wifi_connect();
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&event->ip_info.ip));

        // 🚀 Inicia MQTT somente após IP válido
        mqtt_start();
    }
}

/* ================= MQTT EVENT ================= */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT conectado!");
        mqtt_connected = true;
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT desconectado!");
        mqtt_connected = false;
        break;

    default:
        ESP_LOGI(TAG, "MQTT evento id: %d", event->event_id);
        break;
    }
}

/* ================= WIFI INIT ================= */

void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        NULL);

    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "WiFi iniciado...");
}

/* ================= MQTT ================= */

void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://10.0.0.100:1883",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client,
                                   ESP_EVENT_ANY_ID,
                                   mqtt_event_handler,
                                   NULL);

    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "MQTT iniciado...");
}

/* ================= TASK ================= */

void send_data_task(void *pvParameters)
{
    while (1)
    {
        if (mqtt_connected)
        {
            float temp = 20 + (esp_random() % 100) / 10.0;
            float pressure = 60 + (esp_random() % 100) / 10.0;

            char msg[128];

            snprintf(msg, sizeof(msg),
                     "{\"device\":\"esp32_c6\",\"temperature\":%.2f,\"pressure\":%.2f}",
                     temp, pressure);

            esp_mqtt_client_publish(client, "iot/sensors", msg, 0, 1, 0);

            ESP_LOGI(TAG, "Enviado: %s", msg);
        }
        else
        {
            ESP_LOGW(TAG, "MQTT não conectado, aguardando...");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/* ================= MAIN ================= */

void app_main(void)
{
    nvs_flash_init();

    wifi_init();

    xTaskCreate(send_data_task, "send_data", 4096, NULL, 5, NULL);
}
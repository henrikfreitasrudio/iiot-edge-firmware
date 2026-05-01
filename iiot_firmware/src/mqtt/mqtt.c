#include "mqtt.h"

#include "mqtt_client.h"
#include "esp_log.h"

static const char *TAG = "MQTT";

static esp_mqtt_client_handle_t client;
static bool mqtt_connected = false;

static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
            mqtt_connected = true;
            ESP_LOGI(TAG, "Conectado");
            break;

        case MQTT_EVENT_DISCONNECTED:
            mqtt_connected = false;
            ESP_LOGI(TAG, "Desconectado");
            break;

        default:
            break;
    }
}

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
}

bool mqtt_is_connected(void)
{
    return mqtt_connected;
}

void mqtt_publish(const char *topic, const char *msg)
{
    if (mqtt_connected)
    {
        esp_mqtt_client_publish(client, topic, msg, 0, 1, 0);
    }
}
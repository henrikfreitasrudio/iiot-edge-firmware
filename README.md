# ⚡ IIoT Edge Firmware

Firmware desenvolvido para dispositivos embarcados baseados em ESP32-C6, responsável pela aquisição de dados de sensores e integração com a plataforma **IIoT Edge Platform**.

## 🎯 Objetivo

Este projeto implementa um nó de borda (edge node) capaz de:

* Coletar dados de sensores físicos
* Conectar-se à rede Wi-Fi
* Enviar dados para o backend (Raspberry Pi)
* Operar de forma contínua e confiável

## 🧠 Papel na arquitetura

Este firmware faz parte do ecossistema:

```text
[ ESP32-C6 ] ---> [ IIoT Edge Platform (Raspberry Pi) ] ---> [ Banco de Dados / API ]
```

* O ESP32 atua como **dispositivo de campo**
* O Raspberry Pi atua como **gateway / edge server**

## 🛠️ Tecnologias

* ESP-IDF
* Linguagem C
* FreeRTOS
* HTTP Client
* cJSON

## ⚙️ Funcionalidades

* Conexão Wi-Fi automática
* Leitura periódica de sensores
* Envio de dados via HTTP POST
* Tratamento básico de falhas de conexão
* Estrutura modular para expansão

## 📡 Formato de dados

```json id="fwjson1"
{
  "device_id": "esp32_c6_01",
  "temperature": 25.8,
  "pressure": 101.2,
  "timestamp": "2026-04-21T12:00:00Z"
}
```

## 🔌 Configuração

### Wi-Fi

```c id="fwc1"
#define WIFI_SSID "SEU_WIFI"
#define WIFI_PASS "SUA_SENHA"
```

### Endpoint do servidor

```c id="fwc2"
#define SERVER_URL "http://<raspberry-ip>:8000/sensor-data"
```

## ▶️ Build e Flash

```bash id="fwb1"
idf.py build
idf.py flash
idf.py monitor
```

## 🔄 Fluxo de execução

1. Inicializa sistema (FreeRTOS)
2. Conecta ao Wi-Fi
3. Inicializa sensores
4. Coleta dados
5. Monta payload JSON
6. Envia para API
7. Aguarda intervalo configurado
8. Repete ciclo

## 📁 Estrutura do projeto

```text id="fwstruct1"
main/
 ├── main.c              # Entry point
 ├── wifi.c              # Conexão Wi-Fi
 ├── http_client.c       # Comunicação com backend
 ├── sensor.c            # Interface com sensores
 └── config.h            # Configurações globais
```

## 🔐 Melhorias planejadas

* [ ] Comunicação via MQTT
* [ ] HTTPS (TLS)
* [ ] OTA (Over-The-Air Update)
* [ ] Deep Sleep para economia de energia
* [ ] Provisionamento via BLE
* [ ] Retry inteligente com backoff exponencial

## ⚠️ Considerações

* Projeto voltado para aplicações reais de IIoT
* Priorização de simplicidade e confiabilidade
* Estrutura pensada para escalar para múltiplos dispositivos

## 🔗 Integração

Este firmware foi projetado para funcionar com:

* `iiot-edge-platform` (Raspberry Pi + Docker)
* API REST para ingestão de dados

## 👨‍💻 Contexto

Projeto desenvolvido como parte da evolução profissional em:

* Sistemas embarcados
* Engenharia de software para IIoT
* Integração entre hardware e backend

---

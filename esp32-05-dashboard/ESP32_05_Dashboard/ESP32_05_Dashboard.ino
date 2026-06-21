/*
  ESP32 #5 - Duino-Coin Miner + Wallet Dashboard
  Baseado no Official ESP32 Miner (Duino-Coin Team, MIT License)
  https://github.com/duino-coin/duino-coin

  O QUE ESSE CÓDIGO FAZ:
  - Core 0: minera Duino-Coin normalmente (1 thread, igual aos outros 4 ESPs)
  - Core 1: a cada DASHBOARD_UPDATE_INTERVAL ms, consulta a API pública do
    Duino-Coin e atualiza o display ST7789 240x240 (GMT130) com:
      - Saldo da wallet
      - Hashrate total de todos os rigs
      - Quantidade de rigs minerando

  Edite Settings.h para configurar usuário, Wi-Fi e pinos do display.
*/
#pragma GCC optimize("-Ofast")

#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Settings.h"
#include "MiningJob.h"
#include "WalletDashboard.h"

TaskHandle_t MiningTask;
TaskHandle_t DashboardTask;

MiningConfig *configuration = new MiningConfig(DUCO_USER, RIG_IDENTIFIER, MINER_KEY);
MiningJob *job;

void RestartESP(String msg) {
    #if defined(SERIAL_PRINTING)
        Serial.println(msg);
        Serial.println("Restarting ESP...");
    #endif
    ESP.restart();
    abort();
}

void VerifyWifi() {
    while (WiFi.status() != WL_CONNECTED
            || WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
        #if defined(SERIAL_PRINTING)
            Serial.println("WiFi reconnecting...");
        #endif
        WiFi.disconnect();
        delay(500);
        WiFi.reconnect();
        delay(500);
    }
}

String httpGetString(String URL) {
    String payload = "";
    WiFiClientSecure client;
    HTTPClient https;
    client.setInsecure();

    https.begin(client, URL);
    https.addHeader("Accept", "*/*");

    int httpCode = https.GET();
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        payload = https.getString();
    } else {
        #if defined(SERIAL_PRINTING)
            Serial.printf("Error fetching node from poolpicker: %s\n", https.errorToString(httpCode).c_str());
        #endif
        VerifyWifi();
    }
    https.end();
    return payload;
}

void UpdateHostPort(String input) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, input);
    const char *name = doc["name"];

    configuration->host = doc["ip"].as<String>().c_str();
    configuration->port = doc["port"].as<int>();
    node_id = String(name);

    #if defined(SERIAL_PRINTING)
        Serial.println("Poolpicker selected the best mining node: " + node_id);
    #endif
}

void SelectNode() {
    String input = "";
    int waitTime = 1;

    while (input == "") {
        #if defined(SERIAL_PRINTING)
            Serial.println("Fetching mining node from the poolpicker in " + String(waitTime) + "s");
        #endif
        delay(waitTime * 1000);

        input = httpGetString("https://server.duinocoin.com/getPool");

        waitTime *= 2;
        if (waitTime > 32)
            RestartESP("Node fetch unavailable");
    }
    UpdateHostPort(input);
}

void SetupWifi() {
    #if defined(SERIAL_PRINTING)
        Serial.println("Connecting to: " + String(SSID));
    #endif

    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    VerifyWifi();
    WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), DNS_SERVER);

    #if defined(SERIAL_PRINTING)
        Serial.println("\n\nConectado ao WiFi");
        Serial.println("Rig name: " + String(RIG_IDENTIFIER));
        Serial.println("IP local: " + WiFi.localIP().toString());
    #endif

    SelectNode();
}

void SetupOTA() {
    ArduinoOTA.setHostname(RIG_IDENTIFIER);
    ArduinoOTA.begin();
}

// ----------------------- Core 0: mineração ----------------------- //
void MiningTaskCode(void *parameter) {
    job = new MiningJob(0, configuration);

    for (;;) {
        job->mine();

        if (millis() % 5000 < 50) {
            VerifyWifi();
        }
        ArduinoOTA.handle();
        delay(1);
    }
}

// ------------------ Core 1: wallet dashboard ---------------------- //
void DashboardTaskCode(void *parameter) {
    dashboard_screen_setup();
    dashboard_draw_boot();
    delay(1500);

    for (;;) {
        dashboard_loop();
        delay(50);
    }
}

void setup() {
    #if defined(SERIAL_PRINTING)
        Serial.begin(SERIAL_BAUDRATE);
        Serial.println("\n\nDuino-Coin ESP32 #5 - Miner + Wallet Dashboard");
    #endif

    pinMode(LED_BUILTIN, OUTPUT);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    SetupWifi();
    SetupOTA();

    setCpuFrequencyMhz(240);

    WALLET_ID = String(random(0, 2811)); // Usado para agrupamento de miners na wallet

    // Core 0 = minera | Core 1 = consulta API + display
    xTaskCreatePinnedToCore(MiningTaskCode, "MiningTask", 10000, NULL, 1, &MiningTask, 0);
    xTaskCreatePinnedToCore(DashboardTaskCode, "DashboardTask", 10000, NULL, 1, &DashboardTask, 1);
}

void loop() {
    // Tudo roda nas tasks acima, nada a fazer aqui
    delay(1000);
}

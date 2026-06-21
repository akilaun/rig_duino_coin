// WalletDashboard.h
// Consulta a API pública do Duino-Coin e mostra no display ST7789 240x240 (GMT130):
//   - Saldo da wallet
//   - Hashrate total (soma de todos os miners ativos)
//   - Quantidade de rigs (identificadores únicos) minerando
//
// API usada: https://server.duinocoin.com/users/<username>
// Doc: https://github.com/duino-coin/duco-rest-api
#ifndef WALLET_DASHBOARD_H
#define WALLET_DASHBOARD_H

#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Cores
#define DASH_BG      TFT_BLACK
#define DASH_GOLD    0xFD20
#define DASH_GRAY    0x7BEF
#define DASH_GREEN   0x07E0
#define DASH_RED     0xF800
#define DASH_CYAN    0x07FF
#define DASH_WHITE   TFT_WHITE

TFT_eSPI dashTft = TFT_eSPI();

// Último estado válido (mantém na tela se uma consulta falhar)
float dash_balance = -1;
float dash_hashrate_hs = -1;     // em H/s
unsigned int dash_rig_count = 0;
bool dash_first_draw = true;
bool dash_last_ok = true;

void dashboard_screen_setup() {
    dashTft.init();
    dashTft.setRotation(0); // 240x240, ajuste 0-3 se a imagem ficar invertida
    dashTft.fillScreen(DASH_BG);
}

void dashboard_draw_static_layout() {
    dashTft.fillScreen(DASH_BG);

    // Cabeçalho
    dashTft.setTextColor(DASH_GOLD, DASH_BG);
    dashTft.setTextSize(2);
    dashTft.setCursor(10, 8);
    dashTft.print("Duino-Coin");

    dashTft.setTextColor(DASH_GRAY, DASH_BG);
    dashTft.setTextSize(1);
    dashTft.setCursor(10, 30);
    dashTft.print(String(WALLET_USERNAME));

    dashTft.drawFastHLine(0, 44, 240, DASH_GRAY);

    // Labels (ficam fixos, só o valor é redesenhado depois)
    dashTft.setTextColor(DASH_GRAY, DASH_BG);
    dashTft.setTextSize(1);
    dashTft.setCursor(10, 52);
    dashTft.print("SALDO");

    dashTft.setCursor(10, 122);
    dashTft.print("HASHRATE TOTAL");

    dashTft.setCursor(10, 178);
    dashTft.print("RIGS MINERANDO");

    dashTft.drawFastHLine(0, 110, 240, DASH_GRAY);
    dashTft.drawFastHLine(0, 166, 240, DASH_GRAY);
}

// Desenha apenas a área de valores (evita flicker / redesenhar tudo)
void dashboard_draw_values() {
    // Saldo
    dashTft.fillRect(8, 62, 224, 40, DASH_BG);
    dashTft.setTextColor(DASH_WHITE, DASH_BG);
    dashTft.setTextSize(3);
    dashTft.setCursor(10, 64);
    if (dash_balance >= 0) {
        dashTft.print(String(dash_balance, 2));
    } else {
        dashTft.print("--");
    }
    dashTft.setTextSize(1);
    dashTft.setTextColor(DASH_GRAY, DASH_BG);
    dashTft.setCursor(10, 92);
    dashTft.print("DUCO");

    // Hashrate
    dashTft.fillRect(8, 132, 224, 28, DASH_BG);
    dashTft.setTextColor(DASH_GREEN, DASH_BG);
    dashTft.setTextSize(3);
    dashTft.setCursor(10, 132);
    if (dash_hashrate_hs >= 0) {
        dashTft.print(String(dash_hashrate_hs / 1000.0, 2));
    } else {
        dashTft.print("--");
    }
    dashTft.setTextSize(1);
    dashTft.setTextColor(DASH_GRAY, DASH_BG);
    dashTft.setCursor(150, 144);
    dashTft.print("kH/s");

    // Rigs
    dashTft.fillRect(8, 188, 224, 28, DASH_BG);
    dashTft.setTextColor(DASH_CYAN, DASH_BG);
    dashTft.setTextSize(3);
    dashTft.setCursor(10, 188);
    dashTft.print(String(dash_rig_count));

    // Status / hora da última atualização (rodapé)
    dashTft.fillRect(0, 224, 240, 16, DASH_BG);
    dashTft.setTextSize(1);
    dashTft.setCursor(10, 226);
    if (dash_last_ok) {
        dashTft.setTextColor(DASH_GREEN, DASH_BG);
        dashTft.print("OK - " + String(millis() / 1000) + "s");
    } else {
        dashTft.setTextColor(DASH_RED, DASH_BG);
        dashTft.print("Falha na consulta, tentando de novo...");
    }
}

void dashboard_draw_boot() {
    dashTft.fillScreen(DASH_BG);
    dashTft.setTextColor(DASH_GOLD, DASH_BG);
    dashTft.setTextSize(2);
    dashTft.setCursor(20, 100);
    dashTft.print("Duino-Coin");
    dashTft.setTextSize(1);
    dashTft.setTextColor(DASH_GRAY, DASH_BG);
    dashTft.setCursor(40, 130);
    dashTft.print("Wallet Dashboard");
}

// Faz o GET na API e atualiza dash_balance / dash_hashrate_hs / dash_rig_count
// Retorna true se conseguiu atualizar com sucesso
// Retorna true se conseguiu atualizar com sucesso
bool dashboard_fetch_wallet_data() {
    WiFiClientSecure client;
    HTTPClient https;
    client.setInsecure(); 

    String url = "https://server.duinocoin.com/users/" + String(WALLET_USERNAME);

    if (!https.begin(client, url)) {
        return false;
    }
    
    // Força a resposta em JSON e segue redirecionamentos da API
    https.addHeader("Accept", "application/json");
    https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = https.GET();
    if (httpCode != HTTP_CODE_OK && httpCode != HTTP_CODE_MOVED_PERMANENTLY) {
        #if defined(SERIAL_PRINTING)
            Serial.printf("[Dashboard] HTTP error: %d\n", httpCode);
        #endif
        https.end();
        return false;
    }

    // Lê a resposta inteira para a memória (evita os engasgos do Wi-Fi)
    String payload = https.getString();
    https.end();

    if (payload.length() == 0) {
        return false;
    }

    // Filtro para economizar RAM
    StaticJsonDocument<256> filter;
    filter["success"] = true;
    filter["result"]["balance"]["balance"] = true;
    filter["result"]["miners"][0]["identifier"] = true;
    filter["result"]["miners"][0]["hashrate"] = true;

    DynamicJsonDocument doc(8192);
    DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

    if (err) {
        #if defined(SERIAL_PRINTING)
            Serial.print("[Dashboard] JSON parse error: ");
            Serial.println(err.c_str());
        #endif
        return false;
    }

    if (!doc["success"].as<bool>()) {
        return false;
    }

    dash_balance = doc["result"]["balance"]["balance"].as<float>();

    JsonArray miners = doc["result"]["miners"].as<JsonArray>();

    float totalHashrate = 0;
    String seenIds[60];
    int seenCount = 0;

    for (JsonObject miner : miners) {
        totalHashrate += miner["hashrate"].as<float>();

        String id = miner["identifier"].as<String>();
        bool already = false;
        for (int i = 0; i < seenCount; i++) {
            if (seenIds[i] == id) { already = true; break; }
        }
        if (!already && seenCount < 60) {
            seenIds[seenCount++] = id;
        }
    }

    dash_hashrate_hs = totalHashrate;
    dash_rig_count = seenCount;

    #if defined(SERIAL_PRINTING)
        Serial.printf("[Dashboard] Saldo: %.2f DUCO | Hashrate: %.2f kH/s | Rigs: %d\n",
                      dash_balance, dash_hashrate_hs / 1000.0, dash_rig_count);
    #endif

    return true;
}

// Tarefa principal do core 1: roda em loop, consulta a API a cada
// DASHBOARD_UPDATE_INTERVAL ms e redesenha o display
void dashboard_loop() {
    static unsigned long lastUpdate = 0;
    static bool layoutDrawn = false;

    if (!layoutDrawn) {
        dashboard_draw_static_layout();
        layoutDrawn = true;
    }

    if (dash_first_draw || millis() - lastUpdate >= DASHBOARD_UPDATE_INTERVAL) {
        lastUpdate = millis();
        dash_first_draw = false;

        dash_last_ok = dashboard_fetch_wallet_data();
        dashboard_draw_values();
    }
}

#endif // WALLET_DASHBOARD_H

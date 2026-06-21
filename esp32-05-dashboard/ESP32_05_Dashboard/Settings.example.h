// Settings.example.h
// ESP32 #5 - Minerador + Wallet Dashboard (Duino Coin)
//
// COMO USAR:
// 1. Faça uma cópia deste arquivo e renomeie para "Settings.h"
// 2. Preencha seus dados reais no Settings.h (NUNCA o example.h)
// 3. Settings.h está no .gitignore - suas credenciais não vão pro GitHub
#ifndef SETTINGS_H
#define SETTINGS_H

// ---------------------- General settings ---------------------- //
// Usuário Duino-Coin (esse ESP vai minerar 1 thread nessa conta)
extern char *DUCO_USER = "seu_usuario_duino_coin";
// Mining key (se você configurou uma na wallet, senão deixe "None")
extern char *MINER_KEY = "None";
// Nome desse rig. Use "Auto" para gerar automaticamente
extern char *RIG_IDENTIFIER = "aka_esp32_05";
// Wi-Fi (2.4GHz obrigatório)
extern const char SSID[] = "SEU_WIFI";
extern const char PASSWORD[] = "SUA_SENHA_WIFI";
// -------------------------------------------------------------- //

// -------------------- Advanced options ------------------------ //
// Comente para desabilitar o LED piscando durante a mineração
#define LED_BLINKING

// Comente para desabilitar prints na Serial (útil para debug)
#define SERIAL_PRINTING
#define SERIAL_BAUDRATE 500000

#define LWD_TIMEOUT 30000
// -------------------------------------------------------------- //

// ----------------- Wallet Dashboard (NOVO) ---------------------- //
// Usuário cuja wallet/rigs serão exibidos no display.
// Normalmente igual ao DUCO_USER acima.
#define WALLET_USERNAME DUCO_USER

// Intervalo entre consultas à API (ms). Não recomendado abaixo de 15000 (15s)
// para não sobrecarregar o servidor / sofrer rate limit.
#define DASHBOARD_UPDATE_INTERVAL 20000

// Pinout do display GMT130 (ST7789 240x240) via SPI - ESP32 30 pinos WROOM
// Esse modelo NÃO TEM pino CS físico (só 7 pinos no conector).
// Esses #defines aqui são só referência/documentação - o pinout real do
// display é configurado em User_Setup_GMT130.h (na biblioteca TFT_eSPI)
#define TFT_DASH_MOSI 23
#define TFT_DASH_SCLK 18
#define TFT_DASH_DC    2
#define TFT_DASH_RST   4
#define TFT_DASH_BL   15   // backlight (opcional, pode ligar direto no 3.3V)
// -------------------------------------------------------------- //

// ---------------- Variables and definitions ------------------- //
// Usamos apenas 1 job de mineração (no core 0), por isso CORE = 1
#define CORE 1
#define LED_BUILTIN 2

#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 5

#define SOFTWARE_VERSION "4.3-dashboard"
extern unsigned int hashrate = 0;
extern unsigned int hashrate_core_two = 0;
extern unsigned int difficulty = 0;
extern unsigned long share_count = 0;
extern unsigned long accepted_share_count = 0;
extern String node_id = "";
extern String WALLET_ID = "";
extern unsigned int ping = 0;

IPAddress DNS_SERVER(1, 1, 1, 1); // Cloudflare DNS

#endif  // End of SETTINGS_H

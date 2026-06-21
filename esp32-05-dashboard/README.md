# ESP32 #5 — Duino-Coin Miner + Wallet Dashboard

5º ESP32 da rig: minera 1 thread no core 0 **e** mostra um dashboard ao vivo
da wallet (saldo, hashrate total, rigs ativos) num display GMT130
(ST7789, 240x240, sem pino CS) usando o core 1.

Testado e funcionando ✅

## Arquivos
| Arquivo | O que é |
|---|---|
| `ESP32_05_Dashboard.ino` | Sketch principal (mineração + dashboard) |
| `Settings.example.h` | Modelo de configuração — copie para `Settings.h` e preencha |
| `WalletDashboard.h` | Lógica da API do Duino-Coin + desenho no display |
| `MiningJob.h`, `DSHA1.h`, `Counter.h` | Engine de mineração oficial (sem alterações) |
| `User_Setup_GMT130.h` | Config da biblioteca TFT_eSPI para esse display |
| `teste_tela.ino` | Sketch isolado só pra testar o display (sem Wi-Fi/mineração) |
| `.gitignore` | Garante que `Settings.h` (com suas credenciais) não vá pro Git |

## ⚠️ Antes de subir pro seu repositório
Esse projeto usa um `Settings.h` com seu usuário Duino-Coin, mining key e
**senha do Wi-Fi**. Nunca commite esse arquivo com dados reais.

1. Use sempre o `Settings.example.h` como base (com placeholders)
2. Copie ele pra `Settings.h` localmente e preencha seus dados
3. O `.gitignore` incluído já bloqueia o `Settings.h` de ir pro Git
4. Confira com `git status` antes do primeiro commit se o `Settings.h` não
   aparece na lista de arquivos a serem commitados

## 1. Fiação (ESP32 30 pinos WROOM → GMT130 240x240 ST7789, SPI)

| Display (GMT130) | ESP32 (GPIO) |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SCL / SCK | GPIO18 |
| SDA / MOSI | GPIO23 |
| RES / RST | GPIO4 |
| DC / A0 | GPIO2 |
| BLK | 3.3V (ou GPIO15 p/ controlar brilho por software) |

> O GMT130-V1.0 **não tem pino CS físico** (só 7 pinos no conector) — por
> isso ele não aparece na tabela. No `User_Setup_GMT130.h`, `TFT_CS` fica
> definido como `-1`.

Se usar outros pinos, ajuste os `#define TFT_*` em `User_Setup_GMT130.h`
(é lá que o pinout real é aplicado, em tempo de compilação).

## 2. Bibliotecas necessárias (Arduino IDE → Gerenciador de Bibliotecas)

| Biblioteca | Versão testada | Observação |
|---|---|---|
| `TFT_eSPI` (Bodmer) | **2.5.43** | Versões mais novas deram problema nesse display (tela preta/piscando). Ao instalar pelo Library Manager, use o seletor de versão e escolha `2.5.43` |
| `ArduinoJson` (Benoit Blanchon) | 6.x | Qualquer versão 6 funciona |
| Board package `esp32` (Espressif Systems) | — | Instale via Boards Manager |

## 3. Configurar o TFT_eSPI

A biblioteca precisa saber qual display/pinos usar **em tempo de
compilação** — isso é feito editando um arquivo dentro da própria pasta da
biblioteca, não dá pra configurar isso pelo sketch.

**Localize a pasta da biblioteca TFT_eSPI** (caminho padrão, troque
`SeuUsuario` pelo seu usuário do sistema):

- **Windows:**
  ```
  C:\Users\SeuUsuario\Documents\Arduino\libraries\TFT_eSPI
  ```
  (equivalente a `%USERPROFILE%\Documents\Arduino\libraries\TFT_eSPI`)

- **Linux:**
  ```
  /home/SeuUsuario/Arduino/libraries/TFT_eSPI
  ```
  (equivalente a `~/Arduino/libraries/TFT_eSPI`)

- **macOS** (bônus, caso algum espectador use):
  ```
  /Users/SeuUsuario/Documents/Arduino/libraries/TFT_eSPI
  ```

> Se você usa uma pasta de sketches customizada (Preferences → Sketchbook
> location no Arduino IDE), a pasta `libraries` fica dentro dela em vez do
> local padrão acima. Confira em **Arquivo → Preferências**.

**Depois de localizar a pasta:**
1. Abra o arquivo `User_Setup_Select.h` dentro dela
2. Comente a linha `#include <User_Setup.h>`
3. Copie o `User_Setup_GMT130.h` deste repositório pra dentro da pasta da
   biblioteca e adicione `#include <User_Setup_GMT130.h>` no lugar
   (ou, mais simples: abra o `User_Setup.h` padrão da biblioteca, apague
   tudo e cole o conteúdo do nosso `User_Setup_GMT130.h`)

## 4. Configurar o Settings.h
1. Copie `Settings.example.h` → `Settings.h`
2. Preencha:
   - `DUCO_USER` → seu username Duino-Coin
   - `MINER_KEY` → sua mining key, se tiver uma configurada na wallet
   - `SSID` / `PASSWORD` → seu Wi-Fi (precisa ser 2.4GHz)
   - `RIG_IDENTIFIER` → nome desse rig (ex: "aka esp32 05")
   - `DASHBOARD_UPDATE_INTERVAL` → intervalo entre consultas à API (padrão 20s)

## 5. Testar o display sozinho (recomendado antes de tudo)
Suba o `teste_tela.ino` primeiro (sem mexer no resto). Se ele mostrar as
cores e o texto sem piscar, o hardware/fiação/config da lib estão OK e você
pode seguir pro sketch principal com confiança.

## 6. Upload do sketch principal
Board: `ESP32 Dev Module`. Selecione a porta COM e clique em Upload.

## Como funciona o desafio (1 thread minera, 1 thread consulta a API)
O ESP32 tem 2 núcleos físicos. O código cria duas tasks do FreeRTOS:
- **Core 0** (`MiningTaskCode`): roda `job->mine()` em loop — minera
  normalmente, igual aos outros 4 ESPs, reportando para sua wallet.
- **Core 1** (`DashboardTaskCode`): a cada 20s (configurável), faz um GET em
  `https://server.duinocoin.com/users/<seu_usuario>`, lê a resposta inteira,
  extrai o saldo e a lista de miners, soma o hashrate de todos, conta
  quantos identificadores únicos (rigs) estão ativos, e redesenha só os
  valores na tela (sem recriar o layout inteiro, pra não piscar).

Resultado: você ganha o dashboard sem perder uma rig inteira — esse ESP32
ainda contribui com ~metade do hashrate que teria minerando nas 2 threads.

## Observações sobre a API
- Endpoint usado: `GET /users/<username>` (API REST pública e gratuita do
  Duino-Coin: https://github.com/duino-coin/duco-rest-api)
- O hashrate retornado pela API é por thread/miner reportado, em H/s — o
  código soma tudo e converte pra kH/s na exibição, igual à wallet web.
- "Rigs ativos" = identificadores únicos no array `miners` (cada thread do
  mesmo rig aparece com o mesmo identificador, então são deduplicados).
- Não defina o intervalo de consulta abaixo de ~15s para não sobrecarregar
  o servidor público.
- A resposta é lida inteira pra memória antes do parse (`https.getString()`)
  em vez de parsear direto do stream — mais estável em redes Wi-Fi instáveis.

## Histórico de problemas resolvidos (útil pra quem for reproduzir)
- **Tela preta / piscando sem mostrar nada:** causado por dois fatores
  combinados — (1) o display GMT130 não tem pino CS físico, e o código
  inicial esperava um, e (2) versões mais recentes da TFT_eSPI não
  inicializavam esse painel corretamente. Fixado usando `TFT_CS -1` e
  fixando a biblioteca na versão **2.5.43**.
- Pra isolar o problema, foi usado um sketch de teste só do display
  (`teste_tela.ino`), sem Wi-Fi nem mineração — útil pra descartar se o
  problema é hardware/lib ou interação com o resto do código.

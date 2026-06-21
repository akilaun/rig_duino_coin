# rig_duino_coin

Repositório central do meu projeto de rig de mineração de [Duino-Coin](https://duinocoin.com)
com ESP32 — documentado em vídeo no meu canal do YouTube:
**[youtube.com/@akilaun](https://www.youtube.com/channel/UC6aZfeRosRB-X8SOLEL10vg)**

Esse repositório agrupa todas as atividades, código e documentação da
construção da rig, desde a montagem inicial até as atualizações futuras.

---

## 📺 Episódios relacionados (canal)

| Episódio | Conteúdo |
|---|---|
| #1, #2 | Montagem da rig com os 4 primeiros ESP32 (firmware oficial) |
| **#3** | **5º ESP32 com display dedicado** — minera + mostra dashboard ao vivo da wallet |
| Próximos | Novas atualizações da rig (veja [Roadmap](#-roadmap--próximas-atualizações)) |

---

## 🧩 Arquitetura da rig

A rig hoje é composta por **5 ESP32 WROOM (30 pinos)**, todos minerando para
a mesma wallet (`akilaun`), alimentados por uma fonte chaveada compartilhada.

| Unidade | Função | Firmware | Threads minerando |
|---|---|---|---|
| ESP32 01 | Mineração | Oficial Duino-Coin (fork [akilaun/duino-coin](https://github.com/akilaun/duino-coin)) | 2 |
| ESP32 02 | Mineração | Oficial Duino-Coin (fork [akilaun/duino-coin](https://github.com/akilaun/duino-coin)) | 2 |
| ESP32 03 | Mineração | Oficial Duino-Coin (fork [akilaun/duino-coin](https://github.com/akilaun/duino-coin)) | 2 |
| ESP32 04 | Mineração | Oficial Duino-Coin (fork [akilaun/duino-coin](https://github.com/akilaun/duino-coin)) | 2 |
| ESP32 05 | Mineração + **Dashboard da wallet** (display ST7789 240x240) | Customizado — [`/esp32-05-dashboard`](./esp32-05-dashboard) | 1 (o outro núcleo cuida do display) |

> **Convenção de nomes:** os rigs aparecem na wallet como `aka esp32 0X`
> (01 a 05), seguindo a mesma numeração física da bancada.

---

## 📁 Estrutura do repositório

```
rig_duino_coin/
├── README.md                  ← este arquivo (visão geral do projeto)
└── esp32-05-dashboard/        ← 5º ESP32: minerador + dashboard com display
    ├── README.md               (passo a passo completo dessa unidade)
    ├── ESP32_05_Dashboard.ino
    ├── Settings.example.h
    ├── WalletDashboard.h
    ├── MiningJob.h
    ├── DSHA1.h
    ├── Counter.h
    ├── User_Setup_GMT130.h
    ├── teste_tela.ino
    └── .gitignore
```

Pastas futuras (ESP32 06, sensores, app mobile, etc.) serão adicionadas
conforme o projeto evoluir — veja o roadmap abaixo.

---

## ⚙️ ESP32 01–04 — Mineração padrão

Esses 4 ESP32 rodam o **firmware oficial do Duino-Coin** sem modificações
estruturais, usando o meu fork do repositório oficial:

🔗 **[github.com/akilaun/duino-coin](https://github.com/akilaun/duino-coin)**

Cada um minera com as 2 threads (os 2 núcleos do ESP32), reportando direto
pra wallet. Configuração padrão: editar `Settings.h` do fork com usuário,
Wi-Fi e identificador de cada rig (`aka esp32 01`, `02`, `03`, `04`).

---

## 🖥️ ESP32 05 — Mineração + Dashboard com display

A 5ª unidade é uma evolução: além de minerar (1 thread, no core 0), o
**outro núcleo (core 1)** é dedicado a consultar a API pública da wallet do
Duino-Coin e exibir em tempo real, num display IPS GMT130 (ST7789, 240x240):

- 💰 Saldo da wallet
- ⚡ Hashrate total de todos os rigs ativos
- 🛠️ Quantidade de rigs minerando no momento

Passo a passo completo (fiação, bibliotecas, configuração, troubleshooting)
está documentado em **[`/esp32-05-dashboard/README.md`](./esp32-05-dashboard/README.md)**.

---

## 🛠️ Hardware usado

- 5x ESP32 WROOM 30 pinos (HW-394 V0.4)
- 1x display IPS GMT130 1.3" (ST7789, 240x240, sem pino CS)
- 1x fonte chaveada (alimentação compartilhada dos ESP32)
- Protoboard/perfboard para as conexões
- Jumpers macho-macho / macho-fêmea

---

## 🗺️ Roadmap / próximas atualizações

- [ ] Documentar a montagem física completa (fiação da fonte, distribuição
      de energia entre os 5 ESP32)
- [ ] Avaliar expandir o dashboard com gráfico histórico de hashrate
- [ ] Possível 6º ESP32 / novas funcionalidades (a definir conforme os
      próximos episódios do canal)

Esse roadmap é atualizado conforme novos episódios são gravados — fique de
olho no canal pra acompanhar.

---

## 🔗 Links úteis

- Repositório oficial do Duino-Coin: [github.com/duino-coin/duino-coin](https://github.com/duino-coin/duino-coin)
- Site oficial: [duinocoin.com](https://duinocoin.com)
- Meu fork (firmware dos ESP32 01–04): [github.com/akilaun/duino-coin](https://github.com/akilaun/duino-coin)
- Meu canal no YouTube: [youtube.com/@akilaun](https://www.youtube.com/channel/UC6aZfeRosRB-X8SOLEL10vg)

---

## 📜 Licença

Este projeto é baseado no Official ESP32 Miner do Duino-Coin (MIT licensed,
The Duino-Coin Team, 2019-present). As modificações e adições deste
repositório seguem a mesma licença **MIT**, salvo indicação contrária.

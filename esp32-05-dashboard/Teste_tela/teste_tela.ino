/*
  Teste de Isolamento - Display GMT130 (ST7789)
  Sem Wi-Fi, Sem Mineração. Apenas hardware gráfico.
*/

#include <TFT_eSPI.h>

// Instancia a biblioteca (ela puxa as portas so User_Setup_GMT130.h)
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("\n[TESTE] Iniciando Display...");

  // Inicializa o display
  tft.init();
  tft.setRotation(0); // Ajuste entre 0 e 3 se a imagem ficar de ponta cabeça
  
  // Fundo preto
  tft.fillScreen(TFT_BLACK);
  
  // Desenha o cabeçalho
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Display OK!");
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println("Iniciando teste de cores...");
  delay(1000);
}

void loop() {
  // Desenha blocos de cores puras para testar os pixels e a estabilidade
  tft.fillRect(20, 80, 50, 50, TFT_RED);
  delay(800);
  
  tft.fillRect(90, 80, 50, 50, TFT_GREEN);
  delay(800);
  
  tft.fillRect(160, 80, 50, 50, TFT_BLUE);
  delay(800);

  // Testa a limpeza de tela
  tft.fillScreen(TFT_BLACK);
  delay(500);

  // Traz a mensagem de volta
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 110);
  tft.println("Sem Piscadas?");
  
  delay(2000);
  tft.fillScreen(TFT_BLACK);
}
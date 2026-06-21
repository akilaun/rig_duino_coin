#define USER_SETUP_INFO "GMT130_Setup_Direto"

#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 240
#define TFT_RGB_ORDER TFT_BGR

// O segredo do GMT130: Ele NÃO TEM pino CS
#define TFT_CS   -1

// Pinos SPI (Confirme se os fios estão nestas portas!)
#define TFT_MOSI 23
#define TFT_SCLK 18

// Pinos de Controle
#define TFT_DC    2
#define TFT_RST   4
// (Sem TFT_BL definido aqui, pois você já ligou o fio BLK direto no 3.3V)

// Fontes básicas do sistema
#define LOAD_GLCD
#define LOAD_FONT2

// Reduzi um pouco a frequência para máxima estabilidade na protoboard
#define SPI_FREQUENCY  27000000
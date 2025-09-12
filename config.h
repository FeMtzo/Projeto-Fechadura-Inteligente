
// --- CONFIGURAÇÕES DE HARDWARE ---
// Módulo Leitor RFID MFRC522 (Interface SPI)
#define RFID_SDA_PIN   5  // SS (Slave Select)
#define RFID_RST_PIN   22 // Reset
// Pinos MOSI, MISO e SCK são os padrões do ESP32 para VSPI:
// MOSI: 23, MISO: 19, SCK: 18

// Módulo Relé para acionar a fechadura
#define RELAY_PIN      21

// Teclado Matricial 4x4
// Apenas declaramos as variáveis aqui, avisando que elas existem.
extern const byte KEYPAD_ROWS;
extern const byte KEYPAD_COLS;
extern const byte KEYPAD_ROW_PINS[];
extern const byte KEYPAD_COL_PINS[];

// --- CONFIGURAÇÕES DE ACESSO ---
#define MASTER_PASSWORD "1234" // Senha mestre para acesso
#define MASTER_UID "F3:9F:87:14" // UID da tag RFID mestre (formato: "XX:XX:XX:XX")

// --- CONFIGURAÇÕES DE REDE E TELEGRAM ---
#define WIFI_SSID "CLARO_2G89AE1C"
#define WIFI_PASSWORD "7089AE1C"

#define TELEGRAM_BOT_TOKEN "8225811146:AAFqUO2Nyxz9FfG-9JJW98t--I8sDr_kZNA"
#define TELEGRAM_CHAT_ID "6183959627" // Para receber notificações

// --- CONFIGURAÇÕES DA FECHADURA ---
#define LOCK_OPEN_DURATION 5000 // Tempo em milissegundos que a porta ficará aberta (5s)
#define RELAY_ACTIVE_LEVEL LOW  // Nível lógico para ativar o relé (LOW ou HIGH)
#define ACCESS_FAIL 3000
// --- CONFIGURAÇÕES DE FEEDBACK (LEDs e Buzzer) ---
// #define LED_GREEN_PIN    13 // Pino para o LED Verde
// #define LED_RED_PIN      12 // Pino para o LED Vermelho
// #define LED_BLUE_PIN     14 // Pino para o LED Azul (Estado Ocioso)

#define LED_GREEN_PIN    15 // Pino para o LED Verde
#define LED_RED_PIN      2 // Pino para o LED Vermelho
#define LED_BLUE_PIN     4 // Pino para o LED Azul (Estado Ocioso)
#define BUZZER_PIN       25 // Pino para o Buzzer

// Duração dos sinais de feedback em milissegundos
#define FEEDBACK_DURATION 1000 // 1 segundo
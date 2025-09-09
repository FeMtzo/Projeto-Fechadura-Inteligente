
// --- CONFIGURAÇÕES DE HARDWARE ---
// Módulo Leitor RFID MFRC522 (Interface SPI)
#define RFID_SDA_PIN   5  // SS (Slave Select)
#define RFID_RST_PIN   27 // Reset
// Pinos MOSI, MISO e SCK são os padrões do ESP32 para VSPI:
// MOSI: 23, MISO: 19, SCK: 18

// Módulo Relé para acionar a fechadura
#define RELAY_PIN      26

// Teclado Matricial 4x4
#define KEYPAD_ROWS    4
#define KEYPAD_COLS    4
const byte KEYPAD_ROW_PINS[KEYPAD_ROWS] = {33, 32, 15, 4}; 
const byte KEYPAD_COL_PINS[KEYPAD_COLS] = {14, 12, 13, 25};

// --- CONFIGURAÇÕES DE ACESSO ---
#define MASTER_PASSWORD "1234" // Senha mestre para acesso
#define MASTER_UID "DE:AD:BE:EF" // UID da tag RFID mestre (formato: "XX:XX:XX:XX")

// --- CONFIGURAÇÕES DE REDE E TELEGRAM ---
#define WIFI_SSID "CLARO_2G89AE1C"
#define WIFI_PASSWORD "7089AE1C"

#define TELEGRAM_BOT_TOKEN "8225811146:AAFqUO2Nyxz9FfG-9JJW98t--I8sDr_kZNA"
#define TELEGRAM_CHAT_ID "6183959627" // Para receber notificações

// --- CONFIGURAÇÕES DA FECHADURA ---
#define LOCK_OPEN_DURATION 5000 // Tempo em milissegundos que a porta ficará aberta (5s)
#define RELAY_ACTIVE_LEVEL LOW  // Nível lógico para ativar o relé (LOW ou HIGH)

// --- CONFIGURAÇÕES DE FEEDBACK (LEDs e Buzzer) ---
#define LED_GREEN_PIN    13 // Pino para o LED Verde
#define LED_RED_PIN      12 // Pino para o LED Vermelho
#define LED_BLUE_PIN     14 // Pino para o LED Azul (Estado Ocioso)
#define BUZZER_PIN       25 // Pino para o Buzzer

// Duração dos sinais de feedback em milissegundos
#define FEEDBACK_DURATION 1000 // 1 segundo
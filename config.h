#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
// ============================================================================
// ENUMS & STRUCTS
// ============================================================================

// Enum para representar os estados de feedback de forma legível
enum FeedbackState {
  STATE_IDLE,
  STATE_SUCCESS,
  STATE_FAIL,
  STATE_CADAS,
  STATE_ENROLL_ID_REQUEST,
  STATE_LOCKING,
  STATE_SUCCESS_CADAS,
  STATE_REMOVE,
  STATE_SUCCESS_REMOV,
  STATE_KEYPAD_INPUT,
  STATE_DOOR_OPEN_WAIT,
  STATE_ALARM_UNAUTHORIZED,  
};

// Tipos de acesso possíveis
enum AccessType {
    ACCESS_NONE,
    ACCESS_RFID,
    ACCESS_PASSWORD,
    BIOMETRICS,
    ACCESS_TELEGRAM
};

// Estrutura para gerenciar mensagens do Telegram
struct TelegramMessage {
    String chat_id;
    String text;
    int attempts = 0;
    unsigned long lastAttempt = 0;
    bool pending = false;
};

// ============================================================================
// VARIÁVEIS EXTERNAS
// ============================================================================
extern const String TELEGRAM_CHAT_ID;
extern TelegramMessage pendingMsg;
extern AccessType lastAccessMethod;

// ============================================================================
// FUNÇÕES DO TELEGRAM
// ============================================================================
void requestTelegramMessage(const String &chat_id, const String &text);
void processTelegramMessage();

String accessTypeToString(AccessType method);

void sendTelegramAccessGranted(AccessType method);
void sendTelegramAccessDenied(AccessType method);
void sendTelegramTagCadastrada(const String &uid);
void sendTelegramTagRemovida(const String &uid);
void sendTelegramTagMestra();
void sendTelegramBotStart();
void sendTelegramAlarm();
// ============================================================================
// FUNÇÕES UTILITÁRIAS
// ============================================================================
bool checarPermissao(const String& uid);

// ============================================================================
// CONFIGURAÇÕES DE HARDWARE
// ============================================================================

// --- RFID (Módulo Leitor MFRC522 - Interface SPI) ---
#define RFID_SDA_PIN   5    // SS (Slave Select)
#define RFID_RST_PIN   4   // Reset
// Pinos VSPI padrão ESP32: MOSI (23), MISO (19), SCK (18)

// --- Relé (para acionar a fechadura) ---
#define RELAY_PIN      15

// --- Teclado Matricial 4x4 ---
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
extern byte KEYPAD_ROW_PINS[KEYPAD_ROWS];
extern byte KEYPAD_COL_PINS[KEYPAD_COLS];


// --- Sensor Biométrico AS608 ---
#define BIOMETRICS_RX_PIN 16 // Conectar ao pino TX do sensor
#define BIOMETRICS_TX_PIN 17 // Conectar ao pino RX do sensor

// --- LEDs & Buzzer (Feedback visual/sonoro) ---
#define LED_GREEN_PIN   13
#define LED_RED_PIN     27
#define LED_BLUE_PIN    14
#define BUZZER_PIN      12
#define LCD_SDA_PIN     21
#define LCD_SCL_PIN     22

// --- Sensor de Porta (Reed Switch) ---
#define DOOR_SENSOR_PIN 2
#define DOOR_CLOSED_STATE HIGH  // <<< MUDE PARA HIGH (Porta Fechada = 3.3V)
#define DOOR_OPEN_STATE   LOW   // <<< MUDE PARA LOW (Porta Aberta = GND)

// ============================================================================
// CONFIGURAÇÕES DE ACESSO
// ============================================================================
#define MASTER_UID       "F3:9F:87:14"   // UID da tag RFID mestre
#define PASSWORD "1A2B3C"   // Defina aqui a senha
#define ADMIN_PASSWORD "9999"
// ============================================================================
// CONFIGURAÇÕES DE REDE & TELEGRAM
// ============================================================================
#define WIFI_SSID        "matozo"
#define WIFI_PASSWORD    "matozo2252"
#define TELEGRAM_BOT_TOKEN "8225811146:AAFyoQwxwyT9cApsH6yDvc9XoKz6hr5SMoA"

// ============================================================================
// CONFIGURAÇÕES DE FECHADURA
// ============================================================================
#define LOCK_OPEN_DURATION  5000   // Tempo em ms que a porta ficará aberta
#define RELAY_ACTIVE_LEVEL  LOW    // Nível lógico para ativar o relé
#define ACCESS_FAIL         3000   // Tempo de bloqueio após falha

// ============================================================================
// CONFIGURAÇÕES DE FEEDBACK
// ============================================================================
#define FEEDBACK_DURATION   1000   // Tempo de feedback (1 segundo)

// ============================================================================
#endif // CONFIG_H

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
// ============================================================================
// ENUMS & STRUCTS
// ============================================================================

// Tipos de acesso possíveis
enum AccessType {
    ACCESS_NONE,
    ACCESS_RFID,
    ACCESS_PASSWORD,
    ACCESS_BIOMETRIC,
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

// --- Leitor Biometrico ---

#define BIO_RX          16
#define BIO_TX          17

// --- LEDs & Buzzer (Feedback visual/sonoro) ---
#define LED_GREEN_PIN   13
#define LED_RED_PIN     27
#define LED_BLUE_PIN    14
#define BUZZER_PIN      12
#define LCD_SDA_PIN     21
#define LCD_SCL_PIN     22


// ============================================================================
// CONFIGURAÇÕES DE ACESSO
// ============================================================================
#define MASTER_UID       "F3:9F:87:14"   // UID da tag RFID mestre
#define PASSWORD "1A2B3C"   // Defina aqui a senha

// ============================================================================
// CONFIGURAÇÕES DE REDE & TELEGRAM
// ============================================================================
#define WIFI_SSID        "matozo"
#define WIFI_PASSWORD    "matozo2252"
#define TELEGRAM_BOT_TOKEN "8225811146:AAFqUO2Nyxz9FfG-9JJW98t--I8sDr_kZNA"

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

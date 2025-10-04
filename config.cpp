#include "config.h"
#include <HTTPClient.h>
// --- Chat ID ---
const String TELEGRAM_CHAT_ID = "6183959627";

// --- Teclado Matricial 4x4 ---
byte KEYPAD_ROW_PINS[KEYPAD_ROWS] = {34, 35, 36, 39};  // só entrada -> ótimo para linhas
byte KEYPAD_COL_PINS[KEYPAD_COLS] = {32, 33, 25, 26};  // saídas -> ótimo para colunas

// --- Mensagem pendente ---
TelegramMessage pendingMsg;
AccessType lastAccessMethod = ACCESS_NONE;
// --- Função de envio de mensagem ---
void requestTelegramMessage(const String &chat_id, const String &text) {
    pendingMsg.chat_id = chat_id;
    pendingMsg.text = text;
    pendingMsg.attempts = 0;
    pendingMsg.lastAttempt = 0;
    pendingMsg.pending = true;
}

// --- Conversão AccessType -> String ---
String accessTypeToString(AccessType method) {
    switch (method) {
        case ACCESS_NONE:      return "NONE";
        case ACCESS_RFID:      return "RFID";
        case ACCESS_PASSWORD:  return "TECLADO";
        case ACCESS_BIOMETRIC: return "BIOMETRIC";
        case ACCESS_TELEGRAM:  return "TELEGRAM";
        default:               return "UNKNOWN";
    }
}

// --- Funções do Telegram ---
void sendTelegramAccessGranted(AccessType method) {
    requestTelegramMessage(String(TELEGRAM_CHAT_ID), String("ACESSO PERMITIDO VIA ") + accessTypeToString(method));
}

void sendTelegramAccessDenied(AccessType method) {
    requestTelegramMessage(String(TELEGRAM_CHAT_ID), String("ACESSO NEGADO VIA ") + accessTypeToString(method));
}

void sendTelegramTagCadastrada(const String &uid) {
    requestTelegramMessage(TELEGRAM_CHAT_ID, String("TAG CADASTRADA: ") + uid);
}

void sendTelegramTagRemovida(const String &uid) {
    requestTelegramMessage(TELEGRAM_CHAT_ID, String("TAG REMOVIDA: ") + uid);
}

void sendTelegramTagMestra() {
    requestTelegramMessage(TELEGRAM_CHAT_ID, "TAG MESTRA INSERIDA");
}

void sendTelegramBotStart() {
    requestTelegramMessage(TELEGRAM_CHAT_ID, "Bot iniciou!");
}

// --- Função para checar permissão (exemplo) ---
bool checarPermissao(const String& uid) {
    // Aqui você implementa sua lógica
    if (uid == MASTER_UID) return true;
    return false;
}



void processTelegramMessage() {
    if (!pendingMsg.pending) return;

    unsigned long now = millis();
    if (now - pendingMsg.lastAttempt < 5000) return; // espera 5s entre tentativas

    pendingMsg.lastAttempt = now;
    pendingMsg.attempts++;

    HTTPClient http;
    String url = "https://api.telegram.org/bot" + String(TELEGRAM_BOT_TOKEN) +
                 "/sendMessage?chat_id=" + pendingMsg.chat_id +
                 "&text=" + pendingMsg.text;
    http.begin(url);
    int code = http.GET();
    http.end();

    if (code == 200) {
        Serial.println("Mensagem enviada com sucesso!");
        pendingMsg.pending = false;
    } else {
        Serial.print("Falha ao enviar mensagem. Tentativa ");
        Serial.println(pendingMsg.attempts);
        if (pendingMsg.attempts >= 3) {
            pendingMsg.pending = false; // cancela após 5 tentativas
            Serial.println("Abandonando envio após 3 tentativas.");
        }
    }
}

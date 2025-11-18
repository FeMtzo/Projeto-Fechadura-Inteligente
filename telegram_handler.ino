/*
 * ARQUIVO: telegram_handler.ino
 * DESCRIÇÃO: Conexão Wi-Fi e comunicação com o bot do Telegram.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "config.h"
#include "esp_log.h"


WiFiClientSecure secured_client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, secured_client);
extern bool isLockOpen;

unsigned long lastWifiAttempt = 0;
const unsigned long WIFI_RETRY_INTERVAL = 5000;

unsigned long lastWifiCheck = 0;
bool wifiConnected = false;
bool timeSynced = false;

// --- Configura Wi-Fi e Telegram ---
void setupTelegram() {
  esp_log_level_set("wifi", ESP_LOG_NONE);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  secured_client.setInsecure();
  secured_client.setTimeout(20000);

  // Não vai mais travar esperando, apenas inicia a conexão
}

// --- Verifica estado do WiFi continuamente no loop ---
void checkWiFi() {
  static bool wifiOk = false;
  static bool timeSynced = false;

  if (WiFi.status() == WL_CONNECTED) {
    if (!wifiOk) {
      Serial.println("\nWiFi conectado!");
      sendTelegramBotStart(); 
      wifiOk = true;
    }
    

    if (!timeSynced) {
      // Sincroniza hora só uma vez
      configTime(0, 0, "pool.ntp.org");
      time_t now = time(nullptr);
      if (now > 24 * 3600) {
        Serial.println("Hora sincronizada!");
        timeSynced = true;
      }
    }
  } else {
    if (wifiOk) {
      Serial.println("⚠️ WiFi caiu!");
    }
    wifiOk = false;
    timeSynced = false;

    // tenta reconectar a cada WIFI_RETRY_INTERVAL ms
    if (millis() - lastWifiAttempt >= WIFI_RETRY_INTERVAL) {
      lastWifiAttempt = millis();
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  }
}


// --- Chamado continuamente no loop ---
void handleTelegram() {
  if (WiFi.status() == WL_CONNECTED) {
    processTelegramMessage();  // Só processa se WiFi estiver ok
    // checkTelegramMessages(); // se tiver essa função
  }
}


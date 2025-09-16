/*
 * ARQUIVO: telegram_handler.ino
 * DESCRIÇÃO: Conexão Wi-Fi e comunicação com o bot do Telegram.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "config.h" // Inclua suas definições de pinos, token e chat_id


WiFiClientSecure secured_client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, secured_client);
extern bool isLockOpen;

// --- Variável para detectar mudança de estado da porta ---

// --- Configura Wi-Fi e Telegram ---
void setupTelegram() {
  Serial.println("Conectando ao Wifi SSID: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.println("\nWiFi conectado!");

  // Ignora verificação SSL (teste)
  secured_client.setInsecure();
  secured_client.setTimeout(20000);

  // Sincroniza hora
  configTime(0, 0, "pool.ntp.org");
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println("Hora sincronizada!");
  
  Serial.setDebugOutput(true);
  sendTelegramBotStart();
}

// --- Chamado continuamente no loop ---
void handleTelegram() {
  // Processa envio não bloqueante de mensagens pendentes
  processTelegramMessage();
}



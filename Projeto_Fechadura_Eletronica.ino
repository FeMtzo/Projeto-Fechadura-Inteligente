#include <Arduino.h> // Essencial para PlatformIO com arquivos .cpp

// Bibliotecas (instaladas via Gerenciador de Bibliotecas do PlatformIO)
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include "FS.h"
#include "LittleFS.h"
#include "config.h"
// --- Declaração das funções que estão nos outros arquivos .cpp ---
// Isso informa ao main.cpp que essas funções existem em algum lugar do projeto.
void setupLock();
void openLock();
void handleLockState();
void securityInit();

void setupRFID();
void handleRFID();

void setupKeypad();
void handleKeypad();

void setupTelegram();
void handleTelegram();
void sendTelegramMessage(String message);


// --- Fim das declarações ---

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando a Fechadura Eletrônica (via PlatformIO)...");
  securityInit();
  if (!LittleFS.begin(true)) {
    Serial.println("Erro ao iniciar LittleFS!");
    while(1); // trava o sistema se não conseguir iniciar
  }
  Serial.println("LittleFS iniciado.");

  // Cria o arquivo senhas.txt se não existir
  if (!LittleFS.exists("/senhas.txt")) {
    File f = LittleFS.open("/senhas.txt", "w");
    if (f) f.close();
    Serial.println("Arquivo senhas.txt criado!");
  }
  setupLock();
  setupRFID();
  setupKeypad();
  setupTelegram();
  setupFeedback(); 
  Serial.println("Sistema pronto.");
}

void loop() {
  handleRFID();
  handleKeypad();
  handleTelegram();
  handleLockState();
  handleFeedback();
}

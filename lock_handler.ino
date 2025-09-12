#include "config.h"

// Variáveis de estado da fechadura
bool isLockOpen = false;
unsigned long lockOpenTimestamp = 0;

void setupLock() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Garante que a fechadura comece travada (relé desligado)
  Serial.println("Sistema de trava inicializado.");
}

void openLock() {
  if (!isLockOpen)
    Serial.println("Comando recebido! Abrindo fechadura...");
    digitalWrite(RELAY_PIN, RELAY_ACTIVE_LEVEL);
    isLockOpen = true;
    lockOpenTimestamp = millis();
    signalSuccess();
}

void handleLockState(){
  
  if (!isLockOpen){
    return;
  }
  if (millis() - lockOpenTimestamp >= LOCK_OPEN_DURATION){

    Serial.print("TEMPO ACABOU!! Travando a fechadura...");
    digitalWrite(RELAY_PIN, !RELAY_ACTIVE_LEVEL);
    isLockOpen = false;
  }
}
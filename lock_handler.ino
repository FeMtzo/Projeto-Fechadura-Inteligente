#include "config.h"

// Variáveis de estado da fechadura
bool isLockOpen = false;
unsigned long lockOpenTimestamp = 0;

void setupLock() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, !RELAY_ACTIVE_LEVEL); // garante que comece DESLIGADO
  Serial.println("Sistema de trava inicializado.");
}

void openLock() {
  if (!isLockOpen){
    signalSuccess(); 
    Serial.println("Comando recebido! Abrindo fechadura...");
    digitalWrite(RELAY_PIN, RELAY_ACTIVE_LEVEL);
    isLockOpen = true;
    lockOpenTimestamp = millis();    
}
}

void handleLockState(){
  
  if (!isLockOpen){
    return;
  }
  if (millis() - lockOpenTimestamp >= LOCK_OPEN_DURATION){
    signalLocking();
    Serial.print("TEMPO ACABOU!! Travando a fechadura...");
    digitalWrite(RELAY_PIN, !RELAY_ACTIVE_LEVEL);
    isLockOpen = false;
  }
}


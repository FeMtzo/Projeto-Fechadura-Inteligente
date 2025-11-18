#include "config.h"

extern void signalSuccess();

void setupLock() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, !RELAY_ACTIVE_LEVEL); // garante que comece DESLIGADO
  Serial.println("Sistema de trava inicializado.");
}

void openLock() {
  signalSuccess(); 
  
}

void handleLockState(){
  
}


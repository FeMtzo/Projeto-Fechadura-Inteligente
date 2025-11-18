/*
 * ARQUIVO: door_sensor_handler.ino
 * DESCRIÇÃO: Gerencia o sensor da porta (Reed Switch). (Versão 100% Corrigida)
 */

#include "config.h"

// --- "Importando" Globais ---
extern FeedbackState currentFeedbackState;
extern bool isUnlockAuthorized;
extern void signalAlarm();
extern void signalIdle();
extern void signalLocking();

// --- Variáveis de Debounce ---
int lastReading = DOOR_CLOSED_STATE;
int lastDoorState = DOOR_CLOSED_STATE; // (Mantemos este para o log de MUDANÇA)
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// --- Variáveis para o Monitor Serial de Debug ---
unsigned long lastDebugPrint = 0;
const unsigned long debugPrintInterval = 2000; 

void setupDoorSensor() {
  pinMode(DOOR_SENSOR_PIN, INPUT); 
  lastReading = digitalRead(DOOR_SENSOR_PIN);
  lastDoorState = lastReading; 
  Serial.println("Modulo Sensor de Porta (Reed Switch) inicializado.");
}

void handleDoorSensor() {
  unsigned long now = millis();

  // ----- Bloco de Debug (Imprime estado atual) -----
  if (now - lastDebugPrint >= debugPrintInterval) {
    lastDebugPrint = now; 
    int debugReading = digitalRead(DOOR_SENSOR_PIN); 
    
    if (debugReading == DOOR_CLOSED_STATE) {
      Serial.println("[Sensor Debug] Estado Atual: FECHADA (HIGH)");
    } else {
      Serial.println("[Sensor Debug] Estado Atual: ABERTA (LOW)");
    }
  }
  // ----- Fim do Debug -----

  // --- Lógica de Debounce ---
  int currentReading = digitalRead(DOOR_SENSOR_PIN);

  if (currentReading != lastReading) {
    lastDebounceTime = now;
  }
  lastReading = currentReading; 

  // Só processa se o pino esteve estável
  if ((now - lastDebounceTime) > debounceDelay) {
    
    // ###############################################################
    // ### INÍCIO DA LÓGICA PRINCIPAL (CORRIGIDA) ###
    // ###############################################################
    
    // A lógica foi movida para FORA do "if (leitura mudou)"
    // Agora, ela verifica o estado atual do pino EM TODO LOOP
    
    if (currentReading == DOOR_OPEN_STATE) {
      // --- A PORTA ESTÁ ABERTA (LOW) ---

      if (isUnlockAuthorized) {
        // Cenário A: Abertura OK.
        // A porta foi aberta após um sucesso.
        if (currentFeedbackState != STATE_DOOR_OPEN_WAIT) {
           Serial.println("Sensor: Abertura autorizada detectada.");
           isUnlockAuthorized = false; // Consome a autorização
           currentFeedbackState = STATE_DOOR_OPEN_WAIT; // Muda para "esperando fechar"
        }
      } else {
        // Cenário C: Abertura NÃO AUTORIZADA!
        // Se a porta está aberta E não foi autorizada,
        // E o sistema NÃO ESTÁ no alarme, dispare o alarme.
        if (currentFeedbackState == STATE_IDLE || currentFeedbackState == STATE_LOCKING) {
          Serial.println("Sensor: ALARME! Abertura nao autorizada!");
          signalAlarm();
        }
      }
    } 
    else { 
      // --- A PORTA ESTÁ FECHADA (HIGH) ---
      
      if (currentFeedbackState == STATE_DOOR_OPEN_WAIT) {
        // Estávamos com a porta aberta (autorizada) e agora fechou.
        Serial.println("Sensor: Porta fechada, sinalizando para trancar.");
        signalLocking(); // Ativa o estado de travamento
      }
    }
    
    // (Esta parte é opcional, apenas para o log de "MUDANÇA")
    if (currentReading != lastDoorState) {
        if (currentReading == DOOR_OPEN_STATE) {
            Serial.println("Sensor: MUDANCA -> Porta Abriu (LOW).");
        } else {
            Serial.println("Sensor: MUDANCA -> Porta Fechou (HIGH).");
        }
        lastDoorState = currentReading; // Atualiza o estado
    }
    
    // ###############################################################
    // ### FIM DA LÓGICA PRINCIPAL ###
    // ###############################################################
  }
}
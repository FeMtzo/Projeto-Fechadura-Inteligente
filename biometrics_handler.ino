#include <Adafruit_Fingerprint.h>
#include "config.h"

// --- "Importando" funções e variáveis de outros arquivos ---
extern void signalSuccess();
extern void signalFail();
extern AccessType lastAccessMethod;

// --- Configuração do Sensor ---
HardwareSerial mySerial(2); // Usa a UART2
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// --- Controle de Debounce ---
unsigned long lastFingerTry = 0;
const unsigned long FINGER_READ_COOLDOWN = 2000; // 2 segundos de espera

void setupBiometrics() {
  // Inicia a Serial2 nos pinos definidos
  mySerial.begin(57600, SERIAL_8N1, BIOMETRICS_RX_PIN, BIOMETRICS_TX_PIN);

  finger.begin(57600);
  delay(500);
  
  if (finger.verifyPassword()) {
    Serial.println("Sensor Biometrico AS608 inicializado.");
  } else {
    Serial.println("Sensor Biometrico AS608 nao encontrado!");
    // (Aqui poderíamos ter um sinal de falha crítica, como piscar o LED vermelho)
  }
}

// Converte o código de erro da biblioteca em uma mensagem legível
String getFingerprintError(int p) {
  switch (p) {
    case FINGERPRINT_OK:              return "OK";
    case FINGERPRINT_PACKETRECIEVEERR:return "Erro de comunicacao";
    case FINGERPRINT_NOFINGER:        return "Sem dedo";
    case FINGERPRINT_IMAGEFAIL:       return "Falha na imagem";
    case FINGERPRINT_IMAGEMESS:       return "Imagem muito baguncada";
    case FINGERPRINT_FEATUREFAIL:     return "Nao encontrou features";
    case FINGERPRINT_INVALIDIMAGE:    return "Imagem invalida";
    case FINGERPRINT_ENROLLMISMATCH:  return "Digitais nao batem";
    case FINGERPRINT_BADLOCATION:     return "ID invalido";
    case FINGERPRINT_FLASHERR:        return "Erro na memoria flash";
    default:                          return "Erro desconhecido";
  }
}

// Função principal de verificação (não-bloqueante)
void handleBiometrics() {
  // 1. Verifica se o sensor está pronto para uma nova leitura
  if (millis() - lastFingerTry < FINGER_READ_COOLDOWN) {
    return; // Ainda em tempo de espera
  }

  // 2. Tenta capturar uma imagem da digital
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    return; // Sem dedo no sensor, sai da função
  }

  // Se chegou aqui, um dedo foi detectado
  Serial.println("Digital detectada. Processando...");

  // 3. Converte a imagem em um modelo
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println(getFingerprintError(p));
    return;
  }

  // 4. Procura o modelo na memória do sensor
  p = finger.fingerSearch();

  // 5. Processa o resultado
  if (p == FINGERPRINT_OK) {
    // SUCESSO!
    Serial.print("Digital encontrada! ID #");
    Serial.println(finger.fingerID);
    
    lastAccessMethod = BIOMETRICS; // Define o método de acesso
    openLock();               // Sinaliza para o UI e para a trava
    lastFingerTry = millis();      // Inicia o tempo de espera
    
  } else {
    // FALHA!
    Serial.println("Digital nao encontrada no banco de dados.");
    signalFail();                  // Sinaliza falha para o UI
    lastFingerTry = millis();      // Inicia o tempo de espera
  }
}
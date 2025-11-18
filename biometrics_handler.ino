#include <Adafruit_Fingerprint.h>
#include "config.h"

// --- "Importando" funções e variáveis de outros arquivos ---
extern void signalSuccess();
extern void signalFail();
extern AccessType lastAccessMethod;
extern void signalIdle();
extern LiquidCrystal_I2C lcd;
extern void printCenter(String msg, int row);
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
// Esta função é BLOQUEANTE. Ela assume o controle do loop para o cadastro.
void doEnrollmentProcess(uint8_t id) {
  
  // Passo 1: Esperar a primeira leitura
  lcd.clear();
  printCenter("Coloque o dedo", 0);
  printCenter("CADASTRO de ID" + String(id), 1);
  
  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    delay(50); // Evita sobrecarregar o sensor
  }
  
  // Converte a imagem
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    printCenter("Erro cadastro", 0);
    printCenter(getFingerprintError(p), 1);
    delay(2000);
    signalIdle();
    return;
  }

  // Passo 2: Esperar remover o dedo
  lcd.clear();
  printCenter("Remova o dedo", 0);
  delay(1000); // Dá tempo de ler a mensagem
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(50);
  }

  // Passo 3: Esperar a segunda leitura
  lcd.clear();
  printCenter("Coloque o MESMO", 0);
  printCenter("dedo novamente", 1);
  
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    delay(50);
  }

  // Converte a segunda imagem
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    printCenter("Erro cadastro", 0);
    printCenter(getFingerprintError(p), 1);
    delay(2000);
    signalIdle();
    return;
  }

  // Passo 4: Criar e Salvar o Modelo
  lcd.clear();
  printCenter("Criando modelo...", 0);
  
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    printCenter("Erro cadastro", 0);
    printCenter(getFingerprintError(p), 1);
    delay(2000);
    signalIdle();
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    printCenter("CADASTRADO", 0);
    printCenter("COM SUCESSO!", 1);
  } else {
    lcd.clear();
    printCenter("Erro ao Salvar", 0);
    printCenter(getFingerprintError(p), 1);
  }

  delay(2000); // Tempo para ler a mensagem final
  signalIdle();  // Retorna ao estado ocioso
}
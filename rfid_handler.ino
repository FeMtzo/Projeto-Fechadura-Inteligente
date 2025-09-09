/*
 * ARQUIVO: rfid_handler.ino
 * DESCRIÇÃO: Funções para inicializar e gerenciar o leitor RFID.
 */
 
#include <SPI.h>
#include <MFRC522.h>

// // Cria uma instância do leitor MFRC522
// MFRC522 rfid(RFID_SDA_PIN, RFID_RST_PIN);

// Variáveis para evitar leituras repetidas da mesma tag
long lastReadTime = 0;
String lastReadUID = "";
#define READ_DEBOUNCE_TIME 2000 // Aguarda 2 segundos antes de ler a mesma tag novamente

void setupRFID() {
  SPI.begin(); // Inicia o barramento SPI
  rfid.PCD_Init(); // Inicia o leitor MFRC522
  Serial.println("Módulo RFID inicializado. Aproxime uma tag...");
}

void handleRFID() {
  // Verifica se uma nova tag está presente E se a leitura foi bem-sucedida
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    
    String currentUID = "";
    // O UID é um array de bytes, vamos convertê-lo para uma String
    for (byte i = 0; i < rfid.uid.size; i++) {
      // Adiciona um zero à esquerda se o byte for menor que 16 (formato 0F em vez de F)
      currentUID += (rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      currentUID += String(rfid.uid.uidByte[i], HEX);
    }
    currentUID.toUpperCase(); // Converte para maiúsculas para padronizar
    currentUID.trim(); // Remove espaços extras

    // Lógica de "debounce" para evitar múltiplas ações com uma única aproximação
    if (currentUID == lastReadUID && (millis() - lastReadTime < READ_DEBOUNCE_TIME)) {
      // Mesma tag, lida há pouco tempo. Ignora.
      rfid.PICC_HaltA(); // Coloca a tag em modo de espera
      rfid.PCD_StopCrypto1();
      return;
    }

    Serial.print("Tag detectada! UID: ");
    Serial.println(currentUID);

    // Compara o UID lido com o UID mestre definido em config.h
    if (currentUID == String(MASTER_UID)) {
      Serial.println("ACESSO PERMITIDO!");
      openLock(); // Chama a função do lock_handler para abrir a fechadura
    } else {
      Serial.println("ACESSO NEGADO!");
      signalFail(); // Chama a função de feedback para sinalizar falha
    }
    
    // Atualiza as variáveis de controle de debounce
    lastReadUID = currentUID;
    lastReadTime = millis();

    // Finaliza a leitura da tag atual para permitir a leitura de novas tags
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
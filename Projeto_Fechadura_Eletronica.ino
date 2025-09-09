// #include <Arduino.h> // Essencial para PlatformIO com arquivos .cpp

// // Bibliotecas (instaladas via Gerenciador de Bibliotecas do PlatformIO)
// #include <SPI.h>
// #include <MFRC522.h>
// #include <Keypad.h>
// #include <WiFi.h>
// #include <UniversalTelegramBot.h>

// // --- Declaração das funções que estão nos outros arquivos .cpp ---
// // Isso informa ao main.cpp que essas funções existem em algum lugar do projeto.
// void setupLock();
// void openLock();
// void handleLockState();

// void setupRFID();
// void handleRFID();

// void setupKeypad();
// void handleKeypad();

// void setupTelegram();
// void handleTelegram();
// void sendTelegramMessage(String message);


// // --- Fim das declarações ---

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Iniciando a Fechadura Eletrônica (via PlatformIO)...");

//   setupLock();
//   setupRFID();
//   setupKeypad();
//   setupTelegram();
//   setupFeedback(); 
//   Serial.println("Sistema pronto.");
// }

// void loop() {
//   handleRFID();
//   handleKeypad();
//   handleTelegram();
//   handleLockState();
//   handleFeedback();
// }

/*
 * CÓDIGO TEMPORÁRIO PARA DESCOBRIR O UID DA TAG
 */
#include <SPI.h>
#include <MFRC522.h>

#define RFID_SDA_PIN   5
#define RFID_RST_PIN   27

MFRC522 rfid(RFID_SDA_PIN, RFID_RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Aproxime a tag que deseja cadastrar...");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("UID da Tag: ");
    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidString += (rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      uidString += String(rfid.uid.uidByte[i], HEX);
    }
    uidString.toUpperCase();
    uidString.trim();
    Serial.println(uidString);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(2000); // Aguarda 2 segundos para a próxima leitura
    Serial.println("\nAproxime a próxima tag...");
  }
}
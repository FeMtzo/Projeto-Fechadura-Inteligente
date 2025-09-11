#include <SPI.h>
#include <MFRC522.h>

#define READ_DEBOUNCE_TIME 500

MFRC522 rfid(RFID_SDA_PIN, RFID_RST_PIN);

bool aguardandoAcao = false;       // Esperando a próxima leitura após mestra
bool cadastroAtivo = false;        
bool apagarAtivo = false;          
bool aguardandoConfirmacao = false; // Confirmação mestra
String uidParaCadastrar = "";     
String uidParaRemover = "";       
long lastReadTime = 0;
String lastReadUID = "";

void setupRFID() {
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Módulo RFID inicializado. Aproxime uma tag...");
}

void handleRFID() {
    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial()) return;

    byte uidLength = rfid.uid.size;
    String currentUID = "";
    for (byte i = 0; i < uidLength; i++) {
        if (rfid.uid.uidByte[i] < 0x10) currentUID += "0";
        currentUID += String(rfid.uid.uidByte[i], HEX);
        if (i < uidLength - 1) currentUID += ":";
    }
    currentUID.toUpperCase();

    if (currentUID == lastReadUID && (millis() - lastReadTime < READ_DEBOUNCE_TIME)) {
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        return;
    }
    lastReadUID = currentUID;
    lastReadTime = millis();

    Serial.print("Tag detectada! UID: "); Serial.println(currentUID);

    // ---- CONFIRMAÇÃO MESRA ----
    if (aguardandoConfirmacao && currentUID.equals(MASTER_UID)) {
        if (cadastroAtivo) {
            addSenha(uidParaCadastrar);
            Serial.println("Cadastro concluído: " + uidParaCadastrar);
            uidParaCadastrar = "";
            cadastroAtivo = false;
        } else if (apagarAtivo) {
            if (checkSenha(uidParaRemover)) {
                removeSenha(uidParaRemover);
                Serial.println("Tag removida: " + uidParaRemover);
            } else {
                Serial.println("Essa tag não está cadastrada!");
            }
            uidParaRemover = "";
            apagarAtivo = false;
        }
        aguardandoConfirmacao = false;
        return;
    }

    // ---- TAG MESTRE ----
    if (currentUID.equals(MASTER_UID)) {
        if (!aguardandoAcao) {
            // Primeira leitura da mestra → aguarda próxima ação
            aguardandoAcao = true;
            Serial.println("Tag mestra detectada. Aguarde próxima tag para decidir ação.");
            return;
        } else {
            // Mestra de novo → modo apagar
            apagarAtivo = true;
            aguardandoAcao = false;
            uidParaRemover = "";
            Serial.println("Modo apagar ativado! Aproxime a tag a ser removida.");
            return;
        }
    }

    // ---- TAG NÃO MESTRE ----
    if (aguardandoAcao) {
        // Próxima leitura após mestra → inicia cadastro
        if (!checkSenha(currentUID)) {
            cadastroAtivo = true;
            uidParaCadastrar = currentUID;
            aguardandoConfirmacao = true;
            Serial.println("Modo de cadastro iniciado! Tag preparada para cadastro: " + uidParaCadastrar);
            Serial.println("Aproxime a tag mestra para confirmar o cadastro.");
        } else {
            Serial.println("Esta tag já está cadastrada!");
        }
        aguardandoAcao = false;
        return;
    }

    // Modo apagar → prepara tag para remoção
    if (apagarAtivo && uidParaRemover == "") {
        if (checkSenha(currentUID)) {
            uidParaRemover = currentUID;
            aguardandoConfirmacao = true;
            Serial.println("Tag preparada para apagar: " + uidParaRemover);
            Serial.println("Aproxime a tag mestra para confirmar a remoção.");
        } else {
            Serial.println("Essa tag não está cadastrada!");
        }
        return;
    }

    // Acesso normal
    if (checkSenha(currentUID)) {
        Serial.println("ACESSO PERMITIDO!");
        openLock();
    } else {
        Serial.println("ACESSO NEGADO!");
        signalFail();
    }
}

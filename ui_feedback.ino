/*
 * ARQUIVO: ui_feedback.ino
 * DESCRIÇÃO: Funções para controlar o feedback visual (LEDs) e sonoro (Buzzer).
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Enum para representar os estados de feedback de forma legível
enum FeedbackState {
  STATE_IDLE,
  STATE_SUCCESS,
  STATE_FAIL,
  STATE_CADAS,
  STATE_LOCKING,
  STATE_SUCCESS_CADAS,
  STATE_REMOVE,
  STATE_SUCCESS_REMOV,
  STATE_KEYPAD_INPUT
};

extern String enteredPassword;
extern bool keypadDisplayNeedsUpdate;
// Variáveis de estado para o controle não-bloqueante
FeedbackState currentFeedbackState = STATE_IDLE;
unsigned long feedbackTimestamp = 0;
unsigned long blinkTime = 0;

// Variáveis globais para piscar LED azul
bool cadasPiscarAtivo = false;
unsigned long blinkTimestamp = 0;
int blinkCount = 0;
const int totalBlinks = 8;
const unsigned long blinkInterval = 200; // 200ms
extern bool isLockOpen;
unsigned long now = millis();

// Variáveis globais para controlar o piscar
bool lockingPiscarAtivo = false;
unsigned long lockingBlinkTime = 0;
bool lockingLedState = false;
int lockingBlinkCount = 0;
const int lockingTotalBlinks = 4;
const unsigned long lockingBlinkInterval = 1000; // 1 segundo
bool removePiscarAtivo = false;

// Controle do feedback de sucesso
bool successHandled = false;        // Marca se a mensagem de sucesso já foi enviada
bool successCadasHandled = false;

bool cadasHandled = false;
bool removeHandled = false;
bool successRemoveHandled = false;


unsigned long lockOpenedAt = 0; 
// Controle do feedback de falha
bool failHandled = false; 

// Controle LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // endereço 0x27 é o mais comum
unsigned long tempoMensagem = 0;
bool mostrandoMsg = false;

void setupFeedback() {
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);   // SDA, SCL do ESP32
  lcd.init();           
  lcd.backlight();      
  printCenter("BEM VINDO!", 0);
  delay(3000);  // espera 3 segundos apenas na inicialização
  

  // Inicia o sistema no estado Ocioso
  signalIdle();
  Serial.println("Módulo de Feedback (UI) inicializado.");
}

// Função chamada continuamente no loop principal para gerenciar os temporizadores
void handleFeedback() {
    unsigned long now = millis();

    switch(currentFeedbackState) {
        case STATE_IDLE:
            printCenter("AGUARDANDO", 0);
            printCenter("INTERACAO", 1);
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_BLUE_PIN, HIGH); // Liga o LED azul para indicar "pronto"
            digitalWrite(BUZZER_PIN, LOW);
            return;

        case STATE_SUCCESS:
            if (!successHandled) { // variável para evitar múltiplos envios
                lcd.clear();

                printCenter("SUCESSO", 0);
                printCenter("ABRINDO", 1);
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                digitalWrite(LED_GREEN_PIN, HIGH);
                digitalWrite(BUZZER_PIN, HIGH);
                sendTelegramAccessGranted(lastAccessMethod); // agenda uma vez
                lockOpenedAt = millis();
                successHandled = true; // marca que já enviou
            }

            // Mantém o LED ligado pelo tempo de abertura da fechadura
            if (millis() - lockOpenedAt >= LOCK_OPEN_DURATION) {
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(BUZZER_PIN, LOW);
                signalIdle();
            }
            break;

        case STATE_FAIL:
            if (!failHandled) { // evita múltiplos envios
                lcd.clear();

                printCenter("ACESSO", 0);
                printCenter("NEGADO", 1);
                // Se acabou de entrar no estado, inicializa o timestamp
                feedbackTimestamp = now;
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                digitalWrite(LED_RED_PIN, HIGH);
                digitalWrite(BUZZER_PIN, HIGH);

                sendTelegramAccessDenied(lastAccessMethod); // agenda uma vez
                failHandled = true; // marca que já enviou
            }

            // Mantém o LED e buzzer ligados enquanto o tempo não acabar
            if (now - feedbackTimestamp >= FEEDBACK_DURATION) {
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(BUZZER_PIN, LOW);
                signalIdle();
                feedbackTimestamp = 0; // reseta para próxima vez
                failHandled = false;    // libera para próxima falha
            }
            break;

        case STATE_CADAS:
            if(!cadasHandled){
            lcd.clear();
            printCenter("CADASTRANDO", 0);
            printCenter("USUARIO", 1);
            cadasHandled = true;
            }
            // Inicia o piscar apenas uma vez
            if (!cadasPiscarAtivo) {
                cadasPiscarAtivo = true;
                blinkCount = 0;
                blinkTimestamp = now;
                digitalWrite(LED_BLUE_PIN, LOW);
            }

            // Alterna LED baseado no tempo
            if (cadasPiscarAtivo && now - blinkTimestamp >= blinkInterval) {
                blinkTimestamp = now;
                digitalWrite(LED_BLUE_PIN, !digitalRead(LED_BLUE_PIN));
                blinkCount++;
                if (blinkCount >= totalBlinks * 2) {
                    digitalWrite(LED_BLUE_PIN, LOW);
                    cadasPiscarAtivo = false;
                    signalIdle();
                }
            }
            break;

        case STATE_REMOVE:
            if(!removeHandled){
            lcd.clear();
            printCenter("REMOVENDO", 0);
            printCenter("USUARIO", 1);
            removeHandled = true;
            }
            // Inicia o piscar apenas uma vez
            if (!cadasPiscarAtivo) {
                cadasPiscarAtivo = true;
                blinkCount = 0;
                blinkTimestamp = now;
                digitalWrite(LED_BLUE_PIN, LOW);
                digitalWrite(LED_RED_PIN, LOW);
            }

            // Alterna LED baseado no tempo
            if (cadasPiscarAtivo && now - blinkTimestamp >= blinkInterval) {
                blinkTimestamp = now;
                digitalWrite(LED_BLUE_PIN, !digitalRead(LED_BLUE_PIN));
                digitalWrite(LED_RED_PIN, !digitalRead(LED_RED_PIN));
                blinkCount++;
                if (blinkCount >= totalBlinks * 2) {
                    digitalWrite(LED_BLUE_PIN, LOW);
                    digitalWrite(LED_RED_PIN, LOW);
                    cadasPiscarAtivo = false;
                    signalIdle();
                }
            }
            break;

        case STATE_LOCKING:
            printCenter("TRAVANDO", 0);
            printCenter("PORTA", 1);
            digitalWrite(LED_GREEN_PIN, LOW);
            if (!lockingPiscarAtivo) {
                lockingPiscarAtivo = true;
                lockingBlinkCount = 0;
                lockingBlinkTime = now;
                lockingLedState = false;
                digitalWrite(LED_RED_PIN, LOW);
            }

            if (lockingPiscarAtivo && now - lockingBlinkTime >= lockingBlinkInterval) {
                lockingBlinkTime = now;
                lockingLedState = !lockingLedState;
                digitalWrite(LED_RED_PIN, lockingLedState);
                lockingBlinkCount++;
                if (lockingBlinkCount >= lockingTotalBlinks * 1) {
                    digitalWrite(LED_RED_PIN, LOW);
                    lockingPiscarAtivo = false;
                    signalIdle();
                }
            }
            break;

        case STATE_SUCCESS_CADAS:
            if(!successCadasHandled){
            lcd.clear();
            printCenter("CADASTRADO", 0);
            printCenter("COM SUCESSO", 1);
            successCadasHandled = true;
            }
            if (now - feedbackTimestamp >= 250) { // intervalo de 250ms entre os piscas
            feedbackTimestamp = now;
            static int blinkCount = 0;
            static bool ledState = false;

            if (blinkCount < 8) { // 8 mudanças = 4 piscadas
                ledState = !ledState;
                digitalWrite(LED_GREEN_PIN, ledState ? HIGH : LOW);
                digitalWrite(LED_BLUE_PIN,  ledState ? HIGH : LOW);
                blinkCount++;
            } else {
                // terminou os 4 piscas
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                blinkCount = 0;
                signalIdle();
            }
            }
            break;

        case STATE_SUCCESS_REMOV:
            if(!successRemoveHandled){
            lcd.clear();
            printCenter("REMOVIDO", 0);
            printCenter("COM SUCESSO", 1);
            successRemoveHandled = true;
            }
            if (now - feedbackTimestamp >= 250) { // intervalo de 250ms entre os piscas
            feedbackTimestamp = now;
            static int blinkCount = 0;
            static bool ledState = false;

            if (blinkCount < 8) { // 8 mudanças = 4 piscadas
                ledState = !ledState;
                digitalWrite(LED_GREEN_PIN, ledState ? HIGH : LOW);
                digitalWrite(LED_BLUE_PIN,  ledState ? HIGH : LOW);
                blinkCount++;
            } else {
                // terminou os 4 piscas
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                blinkCount = 0;
                signalIdle();
            }
            }
            break;

        case STATE_KEYPAD_INPUT:
            if (keypadDisplayNeedsUpdate) {
                printCenter("Digite a senha", 0);
                
                // Limpa a segunda linha antes de redesenhar os asteriscos
                lcd.setCursor(0, 1);
                lcd.print("                ");  
                
                // Desenha os asteriscos
                lcd.setCursor(0, 1);
                for (int i = 0; i < enteredPassword.length(); i++) {
                  lcd.print("*");
                }
                
                // Abaixa a bandeira após a atualização
                keypadDisplayNeedsUpdate = false; 
            }
            break; // O break fica fora do if
    }
}
void printCenter(String msg, int row) {
  int col = (16 - msg.length()) / 2;
  if (col < 0) col = 0;  // evita valor negativo
  lcd.setCursor(col, row);
  lcd.print(msg);
}


// ----- Funções para ATIVAR os sinais -----

void signalIdle() {
  currentFeedbackState = STATE_IDLE;
  lcd.clear();
  
}

void signalSuccess() {
  currentFeedbackState = STATE_SUCCESS;
  successHandled = false;   // <-- reset aqui garante que sempre vai acender de novo
}

void signalFail() {
  currentFeedbackState = STATE_FAIL;
  failHandled = false;
}

void signalCadas() {
  currentFeedbackState = STATE_CADAS;
    // Inicialização de variáveis para piscar no handleFeedback()
  cadasPiscarAtivo = false; // será inicializado lá
  cadasHandled = false;

  } 


void signalLocking() {
  currentFeedbackState = STATE_LOCKING;
  lockingPiscarAtivo = false;
  

}

void successCadas(){
  currentFeedbackState = STATE_SUCCESS_CADAS;
  successCadasHandled = false;
}

void signalRemove() {
  currentFeedbackState = STATE_REMOVE;
  removePiscarAtivo = false;
  removeHandled = false;

}
void sucessRemove() {
  currentFeedbackState = STATE_SUCCESS_REMOV;
  successRemoveHandled = false;
}
void signalKeypadInput() {
  if (currentFeedbackState == STATE_IDLE) { // Só muda de estado se estiver ocioso
    currentFeedbackState = STATE_KEYPAD_INPUT;
    keypadDisplayNeedsUpdate = true; 
  }
  
}
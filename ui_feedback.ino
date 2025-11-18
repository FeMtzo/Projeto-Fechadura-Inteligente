/*
 * ARQUIVO: ui_feedback.ino
 * DESCRIÇÃO: Funções para controlar o feedback visual (LEDs) e sonoro (Buzzer).
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

extern bool keypadDisplayNeedsUpdate;
extern bool isUnlockAuthorized; // <<< ADICIONE
extern FeedbackState currentFeedbackState;

extern String enteredPassword;
extern bool keypadDisplayNeedsUpdate;
extern LiquidCrystal_I2C lcd;

extern String enteredPassword;
extern bool keypadDisplayNeedsUpdate;
// Variáveis de estado para o controle não-bloqueante
extern FeedbackState currentFeedbackState;
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

        case STATE_DOOR_OPEN_WAIT:
          printCenter("PORTA ABERTA", 0);
          printCenter("Feche a porta...", 1);
          // Mantém o relé destravado e o LED verde aceso
          digitalWrite(RELAY_PIN, HIGH); 
          digitalWrite(LED_GREEN_PIN, HIGH);
          digitalWrite(LED_RED_PIN, LOW);
          digitalWrite(LED_BLUE_PIN, LOW);
          break;

        case STATE_ALARM_UNAUTHORIZED:
          printCenter("!!! ALARME !!!", 0);
          printCenter("PORTA ARROMBADA", 1);
          // Pisca vermelho e soa o buzzer
          if (millis() - blinkTime > 300) { // Pisca rápido
              blinkTime = millis();
              digitalWrite(LED_RED_PIN, !digitalRead(LED_RED_PIN));
              digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
          }
          digitalWrite(LED_GREEN_PIN, LOW);
          digitalWrite(LED_BLUE_PIN, LOW);
          break;

        case STATE_IDLE:
            digitalWrite(RELAY_PIN, LOW);
            printCenter("AGUARDANDO", 0);
            printCenter("INTERACAO", 1);
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_BLUE_PIN, HIGH); // Liga o LED azul para indicar "pronto"
            digitalWrite(BUZZER_PIN, LOW);
            return;

        case STATE_SUCCESS:
          if (!successHandled) { 
              lcd.clear(); 
              printCenter("SUCESSO", 0);
              printCenter("ABRINDO", 1);
              digitalWrite(RELAY_PIN, HIGH); // <<< DESTRAVA A PORTA
              digitalWrite(LED_RED_PIN, LOW);
              digitalWrite(LED_BLUE_PIN, LOW);
              digitalWrite(LED_GREEN_PIN, HIGH);
              digitalWrite(BUZZER_PIN, HIGH);

              sendTelegramAccessGranted(lastAccessMethod);
              lockOpenedAt = millis();
              successHandled = true; 
          }

          // Mantém o som do buzzer SÓ no início
          if (millis() - lockOpenedAt >= 500) { // Desliga buzzer após 0.5s
              digitalWrite(BUZZER_PIN, LOW);
          }

          // Timeout de auto-relock (se o usuário destravou e NÃO abriu a porta)
          if (millis() - lockOpenedAt >= LOCK_OPEN_DURATION) {
              isUnlockAuthorized = false; // Cancela a autorização
              signalLocking(); // Tranca de volta
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
            digitalWrite(RELAY_PIN, LOW);
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

        case STATE_ENROLL_ID_REQUEST:
          printCenter("CADASTRO", 0);
          printCenter("Digite o ID:", 1);
          // O LCD de asteriscos será atualizado pela lógica do keypad
          break;
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
  // VERIFICA SE O ALARME ESTÁ ATIVO
  if (currentFeedbackState == STATE_ALARM_UNAUTHORIZED) {
    // Se sim, o "sucesso" é apenas um RESET DE ALARME.
    // Não muda para STATE_SUCCESS e NÃO autoriza a abertura.
    Serial.println("UI: Sinal de sucesso recebido. Resetando alarme.");
    signalIdle(); // Apenas volta ao estado ocioso.
  } 
  else {
    // Se não, é um SUCESSO normal para abrir a porta.
    currentFeedbackState = STATE_SUCCESS;
    successHandled = false;
    isUnlockAuthorized = true; // Autoriza a abertura
  }
}

void signalFail() {
  // Se o alarme principal (arrombamento) já está tocando,
  // não devemos mostrar "Acesso Negado" nem resetar o alarme.
  if (currentFeedbackState == STATE_ALARM_UNAUTHORIZED) {
    Serial.println("UI: Sinal de falha ignorado (alarme ativo).");
    return; // Simplesmente ignora a falha e mantém o alarme.
  }

  // Se não, é uma falha normal (sem alarme).
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
  lcd.clear();
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
void signalRequestEnrollID() {
  currentFeedbackState = STATE_ENROLL_ID_REQUEST;
  enteredPassword = ""; // Limpa a senha anterior
  keypadDisplayNeedsUpdate = true; // Força a atualização da tela
  lcd.clear();
}

void signalAlarm() {
  currentFeedbackState = STATE_ALARM_UNAUTHORIZED;
  blinkTime = millis(); // Reseta o timer do pisca
  lcd.clear();
  sendTelegramAlarm(); 
}
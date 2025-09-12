/*
 * ARQUIVO: ui_feedback.ino
 * DESCRIÇÃO: Funções para controlar o feedback visual (LEDs) e sonoro (Buzzer).
 */

// Enum para representar os estados de feedback de forma legível
enum FeedbackState {
  STATE_IDLE,
  STATE_SUCCESS,
  STATE_FAIL,
  STATE_CADAS,
  STATE_LOCKING,
  STATE_SUCCESS_CADAS,
  STATE_REMOVE
};

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
unsigned long lockOpenedAt = 0; 
// Controle do feedback de falha
bool failHandled = false; 

void setupFeedback() {
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Inicia o sistema no estado Ocioso
  signalIdle();
  Serial.println("Módulo de Feedback (UI) inicializado.");
}

// Função chamada continuamente no loop principal para gerenciar os temporizadores
void handleFeedback() {
    unsigned long now = millis();

    switch(currentFeedbackState) {
        case STATE_IDLE:
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_BLUE_PIN, HIGH); // Liga o LED azul para indicar "pronto"
            digitalWrite(BUZZER_PIN, LOW);
            return;

        case STATE_SUCCESS:
            if (!successHandled) { // variável para evitar múltiplos envios
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                digitalWrite(LED_GREEN_PIN, HIGH);
                digitalWrite(BUZZER_PIN, HIGH);

                lastAccessMethod = ACCESS_RFID;

                sendTelegramAccessGranted(lastAccessMethod); // agenda uma vez
                lockOpenedAt = millis();
                successHandled = true; // marca que já enviou
            }

            // Mantém o LED ligado pelo tempo de abertura da fechadura
            if (millis() - lockOpenedAt >= LOCK_OPEN_DURATION) {
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(BUZZER_PIN, LOW);
                currentFeedbackState = STATE_IDLE;
                successHandled = false; // reset para próxima vez
            }
            break;

        case STATE_FAIL:
            if (!failHandled) { // evita múltiplos envios
                // Se acabou de entrar no estado, inicializa o timestamp
                feedbackTimestamp = now;
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                digitalWrite(LED_RED_PIN, HIGH);
                digitalWrite(BUZZER_PIN, HIGH);

                lastAccessMethod = ACCESS_RFID;
                sendTelegramAccessDenied(lastAccessMethod); // agenda uma vez
                failHandled = true; // marca que já enviou
            }

            // Mantém o LED e buzzer ligados enquanto o tempo não acabar
            if (now - feedbackTimestamp >= FEEDBACK_DURATION) {
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(BUZZER_PIN, LOW);
                currentFeedbackState = STATE_IDLE;
                feedbackTimestamp = 0; // reseta para próxima vez
                failHandled = false;    // libera para próxima falha
            }
            break;

        case STATE_CADAS:
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
                    currentFeedbackState = STATE_IDLE;
                }
            }
            break;

        case STATE_REMOVE:
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
                    currentFeedbackState = STATE_IDLE;
                }
            }
            break;

        case STATE_LOCKING:
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
                if (lockingBlinkCount >= lockingTotalBlinks * 2) {
                    digitalWrite(LED_RED_PIN, LOW);
                    lockingPiscarAtivo = false;
                    currentFeedbackState = STATE_IDLE;
                }
            }
            break;

            case STATE_SUCCESS_CADAS:
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
                    currentFeedbackState = STATE_IDLE;
                }
            }
            break;

    }
}


// ----- Funções para ATIVAR os sinais -----

void signalIdle() {
  currentFeedbackState = STATE_IDLE;
}

void signalSuccess() {
  currentFeedbackState = STATE_SUCCESS;
  Serial.print("Sinal de sucesso");
}

void signalFail() {
  currentFeedbackState = STATE_FAIL;
}

void signalCadas() {
    currentFeedbackState = STATE_CADAS;
    // Inicialização de variáveis para piscar no handleFeedback()
    cadasPiscarAtivo = false; // será inicializado lá
  } 


void signalLocking() {
    currentFeedbackState = STATE_LOCKING;
    lockingPiscarAtivo = false;
}

void successCadas(){
    currentFeedbackState = STATE_SUCCESS_CADAS;

}

void signalRemove() {
    currentFeedbackState = STATE_REMOVE;
    removePiscarAtivo = false;
}
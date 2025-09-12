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
  STATE_SUCCESS_CADAS
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
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_BLUE_PIN, LOW);
            digitalWrite(LED_GREEN_PIN, HIGH); // Liga o LED verde
            digitalWrite(BUZZER_PIN, HIGH);    // Emite um beep
            // Mantém o LED verde ligado enquanto a trava estiver aberta
            if (!isLockOpen) {
                // Trava fechada → volta ao estado ocioso
                digitalWrite(LED_GREEN_PIN, LOW);
                currentFeedbackState = STATE_IDLE;
            }
            break;

        case STATE_FAIL:
            // Se acabou de entrar no estado, inicializa o timestamp
            if (feedbackTimestamp == 0) {
                feedbackTimestamp = now;
                digitalWrite(LED_GREEN_PIN, LOW);
                digitalWrite(LED_BLUE_PIN, LOW);
                digitalWrite(LED_RED_PIN, HIGH);
                digitalWrite(BUZZER_PIN, HIGH);
            }

            // Mantém o LED e buzzer ligados enquanto o tempo não acabar
            if (now - feedbackTimestamp >= FEEDBACK_DURATION) {
                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(BUZZER_PIN, LOW);
                currentFeedbackState = STATE_IDLE;
                feedbackTimestamp = 0; // reseta para próxima vez
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
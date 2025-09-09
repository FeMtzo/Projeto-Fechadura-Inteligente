/*
 * ARQUIVO: ui_feedback.ino
 * DESCRIÇÃO: Funções para controlar o feedback visual (LEDs) e sonoro (Buzzer).
 */

// Enum para representar os estados de feedback de forma legível
enum FeedbackState {
  STATE_IDLE,
  STATE_SUCCESS,
  STATE_FAIL
};

// Variáveis de estado para o controle não-bloqueante
FeedbackState currentFeedbackState = STATE_IDLE;
unsigned long feedbackTimestamp = 0;

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
  // Se o estado for ocioso, não há nada a fazer aqui
  if (currentFeedbackState == STATE_IDLE) {
    return;
  }

  // Verifica se o tempo do feedback ativo (sucesso ou falha) já terminou
  if (millis() - feedbackTimestamp >= FEEDBACK_DURATION) {
    signalIdle(); // Retorna para o estado ocioso
  }
}

// ----- Funções para ATIVAR os sinais -----

void signalIdle() {
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_BLUE_PIN, HIGH); // Liga o LED azul para indicar "pronto"
  digitalWrite(BUZZER_PIN, LOW);
  currentFeedbackState = STATE_IDLE;
}

void signalSuccess() {
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_BLUE_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, HIGH); // Liga o LED verde
  digitalWrite(BUZZER_PIN, HIGH);    // Emite um beep
  
  currentFeedbackState = STATE_SUCCESS;
  feedbackTimestamp = millis(); // Inicia o temporizador
}

void signalFail() {
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_BLUE_PIN, LOW);
  digitalWrite(LED_RED_PIN, HIGH); // Liga o LED vermelho
  digitalWrite(BUZZER_PIN, HIGH);  // Emite um som contínuo
  
  currentFeedbackState = STATE_FAIL;
  feedbackTimestamp = millis(); // Inicia o temporizador
}
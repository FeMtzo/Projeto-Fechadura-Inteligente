/*
 * ARQUIVO: keypad_handler.ino
 * DESCRIÇÃO: Funções para ler e processar a entrada do teclado matricial.
 */

#include <Keypad.h>
#include "config.h"
#include <LiquidCrystal_I2C.h>
extern LiquidCrystal_I2C lcd;
extern bool keypadDisplayNeedsUpdate;
// Declaramos as funções de sinalização que vamos chamar
extern void signalKeypadInput();
extern void signalSuccess();
extern void signalFail();
extern void signalIdle();

// Mapeamento das teclas do teclado 4x4
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), KEYPAD_ROW_PINS, KEYPAD_COL_PINS, KEYPAD_ROWS, KEYPAD_COLS);

String enteredPassword = "";
const int passwordLength = sizeof(PASSWORD) - 1; // tamanho da senha definida

void setupKeypad() {
  Serial.println("Teclado inicializado.");

  // 🔹 Ajuste importante:
  // Linhas (34, 35, 36, 39) -> somente entrada, SEM pull-up interno
  for (int i = 0; i < KEYPAD_ROWS; i++) {
    pinMode(KEYPAD_ROW_PINS[i], INPUT);
  }

  // Colunas (32, 33, 25, 26) -> saída
  for (int i = 0; i < KEYPAD_COLS; i++) {
    pinMode(KEYPAD_COL_PINS[i], OUTPUT);
  }
}

void handleKeypad() {
  char key = keypad.getKey();
  if (key) {
    // Ao pressionar a primeira tecla, sinaliza para o UI mudar de tela
    if (enteredPassword.length() == 0 && (key != '*' && key != '#')) {
      signalKeypadInput();
    }
    Serial.print("Tecla pressionada: ");
    Serial.println(key);

    if (key == '*') {
      // Limpa senha e sinaliza para o UI voltar ao estado idle
      enteredPassword = "";
      signalIdle();
    }

    else if (key == '#') {
      // Verifica senha
      if (enteredPassword == PASSWORD) {
        lastAccessMethod = ACCESS_PASSWORD;
        Serial.println("Senha correta! Fechadura acionada.");
        openLock();
        // aqui você pode acionar o relé/trava
      } else {
        lastAccessMethod = ACCESS_PASSWORD;
        Serial.println("Senha incorreta!");
        signalFail();
      }
      enteredPassword = "";
      printCenter("Digite a senha", 0);
    } 
    else {
      // Armazena dígitos
      if (enteredPassword.length() < passwordLength) {
        enteredPassword += key;
        keypadDisplayNeedsUpdate = true;      
      }
    }
  }
}

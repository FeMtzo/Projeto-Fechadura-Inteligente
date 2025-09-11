#include <Arduino.h> // É uma boa prática incluir a biblioteca principal do Arduino
#include "config.h"  // Inclui as declarações para garantir consistência

// Aqui definimos as variáveis de fato, alocando memória para elas.
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
const byte KEYPAD_ROW_PINS[KEYPAD_ROWS] = {33, 32, 15, 4};
const byte KEYPAD_COL_PINS[KEYPAD_COLS] = {14, 12, 13, 25};
#pragma once

#include "driver/gpio.h"
#include <stdint.h> // Para usar uint8_t

// =================================================================
// PARÂMETROS GLOBAIS DA APLICAÇÃO
// =================================================================
#define MAX_PASSWORD_LENGTH 10 // Tamanho máximo da senha, agora definido aqui
#define LOCK_OPEN_DURATION_MS 2500 // Tempo que a fechadura fica aberta em milissegundos

// =================================================================
// TAGS DE LOG GLOBAIS
// =================================================================
#define TAG_MAIN "MAIN"
#define TAG_LOCK "LOCK_ACTUATOR"
#define TAG_UI "UI_FEEDBACK"
#define TAG_KEYPAD "KEYPAD_READER"
#define TAG_RFID "RFID_READER"
#define TAG_SECURITY "SECURITY_MANAGER"
#define TAG_WIFI "WIFI_MANAGER"
#define TAG_TELEGRAM "TELEGRAM_BOT"

// =================================================================
// TIPOS DE DADOS GLOBAIS DA APLICAÇÃO (ARQUITETURA DE EVENTOS)
// =================================================================

// Define os possíveis estados da nossa máquina de estados principal
typedef enum {
    STATE_NORMAL,
    STATE_WAITING_FOR_NEW_TAG,
    STATE_WAITING_FOR_CONFIRMATION,
} SystemState_t;

// Define os tipos de eventos que nosso sistema pode ter
typedef enum {
    EVT_KEYPAD_PASSWORD,
    EVT_RFID_TAG_SCANNED,
} EventType_t;

// A struct que será enviada para a fila central. É o nosso "envelope" padronizado.
typedef struct {
    EventType_t type;    // A "etiqueta" que diz o tipo do evento
    uint8_t len;         // Comprimento dos dados (útil para o UID do RFID)
    union {              // Union permite que diferentes dados compartilhem o mesmo espaço de memória
        char password[MAX_PASSWORD_LENGTH + 1];
        uint8_t rfid_uid[8]; // O UID do RC522 tem até 8 bytes (uint64_t)
    } data;              // O "conteúdo" do envelope
} AppEvent_t;

// =================================================================
// CONFIGURAÇÕES DE REDE
// =================================================================
// Nota: Em um produto final, o ideal seria que isso fosse configurável em tempo de execução.
#define WIFI_SSID           "CLARO_2G89AE1C"
#define WIFI_PASSWORD       "7089AE1C"

// =================================================================
// MAPA DE PINOS CENTRALIZADO DO PROJETO
// =================================================================

// --- Componente: Atuador da Fechadura (Relé) ---
#define LOCK_RELAY_PIN      (gpio_num_t)27

// --- Componente: Feedback de UI (LEDs) ---
#define LED_GREEN_PIN       (gpio_num_t)14
#define LED_RED_PIN         (gpio_num_t)12

// --- Componente: Leitor RFID MFRC522 (SPI) ---
#define RFID_MOSI_PIN       (gpio_num_t)23
#define RFID_MISO_PIN       (gpio_num_t)19
#define RFID_SCK_PIN        (gpio_num_t)18
#define RFID_SDA_PIN        (gpio_num_t)21 // CS/SDA do MFRC522
#define RFID_RST_PIN        (gpio_num_t)22

// --- Componente: Teclado de Membrana 4x4 ---
// Pinos das Linhas (INPUT)
#define KEYPAD_ROW_1_PIN    (gpio_num_t)25
#define KEYPAD_ROW_2_PIN    (gpio_num_t)26
#define KEYPAD_ROW_3_PIN    (gpio_num_t)32
#define KEYPAD_ROW_4_PIN    (gpio_num_t)33
// Pinos das Colunas (OUTPUT)
#define KEYPAD_COL_1_PIN    (gpio_num_t)17
#define KEYPAD_COL_2_PIN    (gpio_num_t)16
#define KEYPAD_COL_3_PIN    (gpio_num_t)5
#define KEYPAD_COL_4_PIN    (gpio_num_t)4

// --- Componente: Leitor Biométrico (Exemplo para o futuro) ---
#define FINGERPRINT_RX_PIN  (gpio_num_t)34
#define FINGERPRINT_TX_PIN  (gpio_num_t)13
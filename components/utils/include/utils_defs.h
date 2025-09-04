#pragma once // Diretiva padrão para evitar inclusões múltiplas

// 1. Tags para Logging
#define TAG_MAIN            "MAIN_APP"
#define TAG_RFID            "RFID_READER"
#define TAG_LOCK            "LOCK_ACTUATOR"
#define TAG_UI              "UI_FEEDBACK"

#define TAG_WIFI            "WIFI_MANAGER"

// 2. Credenciais de Rede
#define WIFI_SSID           "CLARO_2G89AE1C"
#define WIFI_PASSWORD       "7089AE1C"

// 3. Definições de Hardware (Pinos GPIO)
// Mapeamento para o leitor RFID MFRC522 (exemplo, ajuste para o seu hardware)
#define RFID_SDA_PIN        21  // Pino I2C SDA
#define RFID_SCL_PIN        22  // Pino I2C SCL
#define RFID_RST_PIN        17  // Pino de Reset do MFRC522

// Mapeamento para o atuador da fechadura
#define LOCK_RELAY_PIN      23 // Pino que controla o led/relé da fechadura

// Mapeamento para UI (Feedback Visual/Sonoro)
#define LED_GREEN_PIN       2
#define LED_RED_PIN         4
#define BUZZER_PIN          19

// 4. Parâmetros de Funcionamento
#define LOCK_OPEN_DURATION_MS 1500 // Tempo que a fechadura fica aberta em milissegundos

// 5. Estruturas de Dados Compartilhadas
typedef struct {
    char uid_str[24]; // Armazena o UID da tag lida como uma string (ex: "AB:CD:EF:12")
    // Futuramente, podemos adicionar mais campos aqui.
} rfid_tag_data_t;

// 6. Enumerações para Estados e Retornos de Função
typedef enum {
    ACCESS_GRANTED,
    ACCESS_DENIED,
    TAG_NOT_FOUND,
    SYSTEM_ERROR
} access_status_t;
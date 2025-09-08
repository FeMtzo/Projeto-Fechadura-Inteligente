#include "keypad_reader.h"
#include "utils_defs.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <string.h>

// Fila para enviar a senha digitada para outras tasks
static QueueHandle_t password_queue;

// Mapeamento das teclas do teclado 4x4
static const char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Arrays para facilitar a iteração sobre os pinos
static const gpio_num_t row_pins[4] = {KEYPAD_ROW_1_PIN, KEYPAD_ROW_2_PIN, KEYPAD_ROW_3_PIN, KEYPAD_ROW_4_PIN};
static const gpio_num_t col_pins[4] = {KEYPAD_COL_1_PIN, KEYPAD_COL_2_PIN, KEYPAD_COL_3_PIN, KEYPAD_COL_4_PIN};

// --- A Tarefa de Varredura do Teclado ---

static void keypad_scan_task(void *pvParameters)
{
    char password_buffer[MAX_PASSWORD_LENGTH + 1];
    uint8_t buffer_pos = 0;
    memset(password_buffer, 0, sizeof(password_buffer));

    while (1)
    {
        ESP_LOGI(TAG_KEYPAD, "--- Novo Ciclo de Varredura ---");
        char pressed_key = '\0';

        for (int col = 0; col < 4; col++)
        {
            ESP_LOGI(TAG_KEYPAD, "Ativando coluna %d (pino %d)", col, col_pins[col]);
            gpio_set_level(col_pins[col], 0);

            // Damos um tempo mínimo para o sinal elétrico se estabilizar
            vTaskDelay(pdMS_TO_TICKS(1)); 

            // Lê e imprime o estado de TODAS as linhas
            int r0 = gpio_get_level(row_pins[0]);
            int r1 = gpio_get_level(row_pins[1]);
            int r2 = gpio_get_level(row_pins[2]);
            int r3 = gpio_get_level(row_pins[3]);
            ESP_LOGI(TAG_KEYPAD, " -> Lendo linhas: L1=%d, L2=%d, L3=%d, L4=%d", r0, r1, r2, r3);

            for (int row = 0; row < 4; row++)
            {
                if (gpio_get_level(row_pins[row]) == 0)
                {
                    pressed_key = key_map[row][col];
                    while(gpio_get_level(row_pins[row]) == 0) {
                        vTaskDelay(pdMS_TO_TICKS(10)); // Espera a tecla ser solta
                    }
                }
            }
            gpio_set_level(col_pins[col], 1);
        }

        if (pressed_key != '\0')
        {
            // (A lógica de processamento da senha continua a mesma aqui)
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aumentamos a pausa para ler o log com calma
    }
}
    void keypad_reader_init(void)
{
    // Configura os pinos das colunas como saída
    for (int i = 0; i < 4; i++) {
        gpio_set_direction(col_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(col_pins[i], 1); // Desativa todas as colunas
    }

    // Configura os pinos das linhas como entrada com pull-up interno
    for (int i = 0; i < 4; i++) {
        gpio_set_direction(row_pins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(row_pins[i], GPIO_PULLUP_ONLY);
    }
    
    // Cria a fila que armazenará a senha final
    password_queue = xQueueCreate(1, sizeof(char) * (MAX_PASSWORD_LENGTH + 1));
    ESP_LOGI(TAG_KEYPAD, "Componente inicializado.");
}

void keypad_reader_start_task(void)
{
    xTaskCreate(keypad_scan_task, "keypad_scan_task", 2048, NULL, 5, NULL);
    ESP_LOGI(TAG_KEYPAD, "Tarefa de varredura iniciada.");
}

QueueHandle_t keypad_reader_get_password_queue(void)
{
    return password_queue;
}
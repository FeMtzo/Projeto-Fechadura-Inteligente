#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Incluindo os cabeçalhos dos nossos componentes
#include "rfid_reader.h"
#include "lock_actuator.h"
#include "ui_feedback.h"
#include "utils_defs.h"
// Ponto de entrada para programas ESP-IDF
extern "C" void app_main(void)
{
    printf("Projeto Fechadura RFID - Inicializando...\n");

    // Aqui vamos inicializar nossos componentes
    // rfid_reader_init();

    lock_actuator_init();
    ui_feedback_init();

   
    while (true) {
        printf("Abrindo a fechadura...\n");
        lock_actuator_open();
        vTaskDelay(pdMS_TO_TICKS(3000));  // Trava aberta por 3 segundos

        printf("Fechando a fechadura...\n");
        lock_actuator_close();
        vTaskDelay(pdMS_TO_TICKS(3000));
        gpio_set_level((gpio_num_t)LED_RED_PIN, 0);  // Trava fechada por 3 segundos
    }

}
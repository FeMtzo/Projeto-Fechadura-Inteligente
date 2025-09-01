#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Incluindo os cabeçalhos dos nossos componentes
#include "rfid_reader.h"
#include "lock_actuator.h"
#include "ui_feedback.h"

// Ponto de entrada para programas ESP-IDF
extern "C" void app_main(void)
{
    printf("Projeto Fechadura RFID - Inicializando...\n");

    // Aqui vamos inicializar nossos componentes
    rfid_reader_init();
    lock_actuator_init();
    ui_feedback_init();

}
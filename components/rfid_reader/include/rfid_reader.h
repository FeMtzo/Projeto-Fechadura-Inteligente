#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @brief Inicializa o leitor RFID, registra os eventos e inicia a varredura em segundo plano.
 *
 * @param event_queue A fila de eventos central da aplicação para onde os dados da tag serão enviados.
 */
void rfid_reader_init(QueueHandle_t event_queue);
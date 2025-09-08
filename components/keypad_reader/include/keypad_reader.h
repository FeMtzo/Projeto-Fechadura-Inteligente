#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define MAX_PASSWORD_LENGTH 10 // Define o tamanho máximo da senha

/**
 * @brief Inicializa os pinos GPIO para o teclado e cria a fila de senhas.
 */
void keypad_reader_init(void);

/**
 * @brief Inicia a tarefa de varredura do teclado.
 */
void keypad_reader_start_task(void);

/**
 * @brief Retorna o handle da fila onde as senhas digitadas serão enviadas.
 *
 * @return O handle da fila (QueueHandle_t).
 */
QueueHandle_t keypad_reader_get_password_queue(void);
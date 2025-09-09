#include "lock_actuator.h"
#include "utils_defs.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "ui_feedback.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// Implementação da função
void lock_actuator_init(void)
{
    ESP_LOGI(TAG_LOCK, "Inicializando o atuador de trava...");

    // Reseta o pino do atuador para garantir um estado limpo
    gpio_reset_pin((gpio_num_t)LOCK_RELAY_PIN);

    // Configura o pino do atuador como saída digital
    gpio_set_direction((gpio_num_t)LOCK_RELAY_PIN, GPIO_MODE_OUTPUT);
}

void lock_actuator_open(void)
{
    ESP_LOGI(TAG_LOCK, "Abrindo a trava...");
    // Ativa o atuador (assumindo que HIGH abre a trava)
    gpio_set_level((gpio_num_t)LOCK_RELAY_PIN, 1);
    gpio_set_level((gpio_num_t)LED_GREEN_PIN, 1); // Acende o LED indicador

}

void lock_actuator_close(void)
{
    ESP_LOGI(TAG_LOCK, "Fechando a trava...");
    gpio_set_level((gpio_num_t)LOCK_RELAY_PIN, 0);
    gpio_set_level((gpio_num_t)LED_GREEN_PIN, 0); // Apaga o LED indicador
    gpio_set_level((gpio_num_t)LED_RED_PIN, 1);   // Acende o LED vermelho ao fechar
    vTaskDelay(pdMS_TO_TICKS(500));                // Mantém o LED vermelho
   // Apaga o LED vermelho
    
}


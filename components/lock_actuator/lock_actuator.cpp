#include "lock_actuator.h"
#include "utils_defs.h"
#include "driver/gpio.h"
#include "esp_log.h"

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
}

void lock_actuator_close(void)
{
    ESP_LOGI(TAG_LOCK, "Fechando a trava...");
    gpio_set_level((gpio_num_t)LOCK_RELAY_PIN, 0);
}


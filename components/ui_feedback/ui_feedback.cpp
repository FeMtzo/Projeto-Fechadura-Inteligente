#include "ui_feedback.h"
#include "utils_defs.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Para usar vTaskDelay e pdMS_TO_TICKS
void ui_feedback_init(void)
{
    ESP_LOGI(TAG_UI, "Inicializando GPIOs para feedback visual.");
    // Configura o pino do LED verde como saída
    gpio_set_direction(LED_GREEN_PIN, GPIO_MODE_OUTPUT);
    // Configura o pino do LED vermelho como saída
    gpio_set_direction(LED_RED_PIN, GPIO_MODE_OUTPUT);
}
// Acende o LED verde para indicar acesso permitido
void ui_feedback_show_access_granted(void)
{
    // Sequência para acesso permitido
    gpio_set_level(LED_GREEN_PIN, 1);
    // A pausa para manter o LED aceso será controlada pelo main,
    // junto com o tempo de acionamento do relé.
    // Aqui apenas ligamos o LED. O main se encarregará de desligá-lo
    // ao chamar lock_actuator_close().
}
// Pisca o LED vermelho para indicar acesso negado
void ui_feedback_show_access_denied(void)
{
    // Sequência para acesso negado: pisca o LED vermelho 3 vezes
    for (int i = 0; i < 3; i++)
    {
        gpio_set_level(LED_RED_PIN, 1); // Acende o LED vermelho
        vTaskDelay(pdMS_TO_TICKS(150)); // Acende por 150 ms
        gpio_set_level(LED_RED_PIN, 0); // Apaga o LED vermelho
        vTaskDelay(pdMS_TO_TICKS(150)); // Pausa entre os piscas
    }
}
// Sinalização para indicar que o modo de cadastro foi ativado
void ui_feedback_enter_registration_mode(void)
{
    // Pisca o LED verde 3 vezes lentamente para indicar modo de cadastro
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_GREEN_PIN, 1); // Acende o LED verde
        vTaskDelay(pdMS_TO_TICKS(300)); // Acende por 300 ms
        gpio_set_level(LED_GREEN_PIN, 0); // Apaga o LED verde
        vTaskDelay(pdMS_TO_TICKS(300)); // Pausa entre os piscas
    }
}
// Sinalização para indicar sucesso no cadastro
void ui_feedback_registration_success(void)
{
    gpio_set_level(LED_GREEN_PIN, 1); // Acende o LED verde
    vTaskDelay(pdMS_TO_TICKS(1500)); // Mantém aceso por 1.5 segundos
    gpio_set_level(LED_GREEN_PIN, 0); // Apaga o LED verde
}
// Sinalização para indicar falha ou cancelamento do cadastro
void ui_feedback_registration_fail(void)
{
    gpio_set_level(LED_RED_PIN, 1); // Acende o LED vermelho
    vTaskDelay(pdMS_TO_TICKS(1500)); // Mantém aceso por 1.5 segundos
    gpio_set_level(LED_RED_PIN, 0); // Apaga o LED vermelho
}
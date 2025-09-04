#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Nossos componentes
#include "lock_actuator.h" // Inclua o componente que queremos usar
#include "utils_defs.h" // Para a tag de log
#include "esp_log.h" // Para logar
#include "wifi_manager.h" // Para gerenciar o WiFi

// Ponto de entrada para programas ESP-IDF
extern "C" void app_main(void)
{
   ESP_LOGI(TAG_MAIN, "Iniciando o sistema de fechadura inteligente...");
   wifi_manager_init();
   wifi_manager_connect();
   lock_actuator_init();

   while (1)
   {
        ESP_LOGI(TAG_MAIN, "Abrindo a fechadura...");
        lock_actuator_open();
        vTaskDelay(pdMS_TO_TICKS(3000)); // Espera 3 segundos

        ESP_LOGI(TAG_MAIN, "Fechando a fechadura...");
        lock_actuator_close();
        vTaskDelay(pdMS_TO_TICKS(3000)); // Espera 3 segundos
   }

}
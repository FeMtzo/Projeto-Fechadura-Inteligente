#include "rfid_reader.h"
#include "rc522.h"
#include "rc522_picc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <string.h>
#include "utils_defs.h" 
static const char *TAG = "RFID_READER";
static rc522_handle_t rfid_handle = NULL;
static QueueHandle_t g_app_queue = NULL;

/**
 * @brief Evento chamado quando há mudança de estado do PICC (tag detectada)
 */
static void rfid_event_handler(void* arg, esp_event_base_t base, int32_t id, void* event_data)
{
    if (id != RC522_EVENT_PICC_STATE_CHANGED) return;

    rc522_picc_state_changed_event_t *ev = (rc522_picc_state_changed_event_t*)event_data;
    if (!ev || !ev->picc) return;

    rc522_picc_t *picc = ev->picc;

    // UID convertido para string legível
    char uid_str[RC522_PICC_UID_STR_BUFFER_SIZE_MAX] = {0};
    if (rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str)) == ESP_OK) {
        ESP_LOGI(TAG, "Tag detectada! UID: %s", uid_str);
    } else {
        ESP_LOGW(TAG, "Falha ao converter UID para string");
    }

    ESP_LOGI(TAG, "Tipo do PICC: %s", rc522_picc_type_name(picc->type));

    // Se houver queue do aplicativo, envia a UID
    if (g_app_queue) {
        uint8_t uid_copy[RC522_PICC_UID_SIZE_MAX] = {0};
        memcpy(uid_copy, picc->uid.value, picc->uid.length);
        xQueueSend(g_app_queue, uid_copy, portMAX_DELAY);
    }
}

/**
 * @brief Inicializa o leitor RC522
 */
void rfid_reader_init(QueueHandle_t app_event_queue)
{
    g_app_queue = app_event_queue;

    rc522_config_t config = {};
    config.poll_interval_ms = 100;                 // polling a cada 100ms
    config.task_stack_size = 2048;                 // stack da task
    config.task_priority = 10;                     // prioridade da task
    config.task_mutex = xSemaphoreCreateMutex();   // mutex interno

    esp_err_t err = rc522_create(&config, &rfid_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar RC522: %d", err);
        return;
    }

    rc522_register_events(rfid_handle, RC522_EVENT_PICC_STATE_CHANGED, rfid_event_handler, NULL);

    rc522_start(rfid_handle);

    ESP_LOGI(TAG, "RC522 inicializado com sucesso");
}

/**
 * @brief Destrói o leitor RC522
 */
void rfid_reader_deinit()
{
    if (rfid_handle) {
        rc522_pause(rfid_handle);
        rc522_destroy(rfid_handle);
        rfid_handle = NULL;
        ESP_LOGI(TAG, "RC522 destruído");
    }
}

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include <string.h>
#include "nvs_flash.h" // Boa prática incluir para códigos de erro

// --- Nossos componentes e definições ---
#include "utils_defs.h"
#include "esp_log.h"
#include "security_manager.h"
#include "rfid_reader.h" // <-- Corrigido
#include "ui_feedback.h"
#include "lock_actuator.h"

// A Fila Central para todos os eventos da aplicação
static QueueHandle_t app_event_queue;

// Variáveis para controlar nossa máquina de estados de cadastro
static SystemState_t current_state = STATE_NORMAL;
static AppEvent_t tag_to_register; // Guarda a tag que estamos prestes a cadastrar

/**
 * @brief Função que bloqueia o programa para cadastrar a primeira Tag Mestra.
 */
void provision_master_tag_mode(void)
{
    ESP_LOGW(TAG_MAIN, "ENTRANDO EM MODO DE CADASTRO DE TAG MESTRA");
    // Aqui você poderia chamar uma função do lcd_feedback para mostrar a mensagem
    // ex: lcd_feedback_display_message("MODO CADASTRO:", "Aproxime a TAG");

    AppEvent_t event;
    while(1) {
        // Espera eternamente pela primeira tag que for aproximada
        if (xQueueReceive(app_event_queue, &event, portMAX_DELAY) == pdPASS) {
            if (event.type == EVT_RFID_TAG_SCANNED) {
                ESP_LOGW(TAG_MAIN, "Tag recebida! Definindo como Mestra...");
                security_manager_set_master_tag(event.data.rfid_uid, event.len);
                ui_feedback_registration_success();
                ESP_LOGW(TAG_MAIN, "Tag Mestra definida! Reiniciando em 3 segundos...");
                vTaskDelay(pdMS_TO_TICKS(3000));
                
                esp_restart(); // Reinicia o ESP32 para operar em modo normal
            }
        }
    }
}

/**
 * @brief Ponto de entrada principal da aplicação.
 */
extern "C" void app_main(void)
{
    ESP_LOGI(TAG_MAIN, "Inicializando componentes...");

    // Cria a fila central de eventos
    app_event_queue = xQueueCreate(5, sizeof(AppEvent_t));

    // O security_manager deve ser o primeiro a inicializar, pois ele prepara o armazenamento
    esp_err_t master_tag_status = security_manager_init(); 
    
    // Inicializa os outros componentes de hardware e lógica
    rfid_reader_init(app_event_queue); // <-- Corrigido
    
    lock_actuator_init(); // Inicializa o atuador de trava
    ui_feedback_init(); // Inicializa o sistema de feedback (LEDs, Buzzer, LCD, etc)

    // Lista as tags já salvas no armazenamento para depuração
    security_manager_list_all_rfid_tags();

    // Verifica se a tag mestra foi encontrada
    if (master_tag_status == ESP_ERR_NOT_FOUND) {
        // Se não, entra em modo de provisionamento e não sai mais desta função
        provision_master_tag_mode();
    }
    
    ESP_LOGI(TAG_MAIN, "Sistema pronto. Operando em modo normal.");
    
    AppEvent_t event; // Variável para receber eventos da fila

    // Loop principal de eventos da aplicação
    while (1) 
    {
        // Espera por um evento chegar na fila (do RFID ou, futuramente, do teclado)
        if (xQueueReceive(app_event_queue, &event, portMAX_DELAY) == pdPASS)
        {
            // Por enquanto, só nos importamos com eventos de RFID
            if (event.type != EVT_RFID_TAG_SCANNED) continue;

            // --- LÓGICA DA MÁQUINA DE ESTADOS ---
            switch (current_state)
            {
                case STATE_NORMAL:
                    if (security_manager_is_master_tag(event.data.rfid_uid, event.len)) {
                        // Se a tag mestra for aproximada, entra em modo de cadastro
                        ESP_LOGW(TAG_MAIN, "Tag Mestra detectada! Entrando em modo de cadastro.");
                        ui_feedback_enter_registration_mode();
                        current_state = STATE_WAITING_FOR_NEW_TAG;
                    } else if (security_manager_verify_rfid(event.data.rfid_uid, event.len)) {
                        // Se for uma tag normal e válida, permite o acesso
                        ESP_LOGW(TAG_MAIN, "Acesso PERMITIDO!");
                        ui_feedback_show_access_granted();
                        lock_actuator_open();
                        vTaskDelay(pdMS_TO_TICKS(LOCK_OPEN_DURATION_MS));
                        lock_actuator_close();
                        gpio_set_level(LED_GREEN_PIN, 0); // Garante que o LED verde apague
                    } else {
                        // Se for uma tag normal e inválida, nega o acesso
                        ESP_LOGE(TAG_MAIN, "Acesso NEGADO!");
                        ui_feedback_show_access_denied();
                    }
                    break;

                case STATE_WAITING_FOR_NEW_TAG:
                    if (security_manager_is_master_tag(event.data.rfid_uid, event.len)) {
                        // Se a mestra for aproximada de novo, cancela a operação
                        ESP_LOGW(TAG_MAIN, "Cadastro cancelado pela Tag Mestra.");
                        ui_feedback_registration_fail();
                        current_state = STATE_NORMAL;
                    } else {
                        // Se uma nova tag for aproximada, guarda ela e espera a confirmação
                        ESP_LOGI(TAG_MAIN, "Nova tag recebida. Aproxime a Tag Mestra para confirmar.");
                        memcpy(&tag_to_register, &event, sizeof(AppEvent_t));
                        current_state = STATE_WAITING_FOR_CONFIRMATION;
                    }
                    break;

                case STATE_WAITING_FOR_CONFIRMATION:
                    if (security_manager_is_master_tag(event.data.rfid_uid, event.len)) {
                        // Se a mestra confirmar, salva a nova tag no armazenamento
                        ESP_LOGW(TAG_MAIN, "Confirmacao recebida! Cadastrando nova tag...");
                        security_manager_add_rfid(tag_to_register.data.rfid_uid, tag_to_register.len);
                        ui_feedback_registration_success();
                        current_state = STATE_NORMAL;
                    } else {
                        // Se qualquer outra tag for aproximada, cancela a operação
                        ESP_LOGE(TAG_MAIN, "Confirmacao falhou! Tag incorreta. Cancelando cadastro.");
                        ui_feedback_registration_fail();
                        current_state = STATE_NORMAL;
                    }
                    break;
            }
        }
    }
}
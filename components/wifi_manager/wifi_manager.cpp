#include "wifi_manager.h"
#include "utils_defs.h"

// Bibliotecas ESP-IDF necessárias
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h" 

// Para gerenciar o estado da conexão
static bool wifi_connected = false;

// função que sera chamada para tratar os eventos do wifi e da rede
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) { // inicializa o wifi
        ESP_LOGI(TAG_WIFI, "Conectando ao WiFi...");
        esp_wifi_connect(); // tenta conectar ao wifi
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) { // desconecta do wifi
        wifi_connected = false;
        esp_wifi_connect(); // tenta reconectar ao wifi
        ESP_LOGI(TAG_WIFI, "Tentando reconectar ao WiFi...");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) { // obtem o ip
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Conectado ao WiFi, endereço IP: " IPSTR, IP2STR(&event->ip_info.ip));
        wifi_connected = true; 
    }
}
void wifi_manager_init(void) {
    // 1. inicializa a flash nvs
    esp_err_t ret = nvs_flash_init(); 
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. configura o cliente seguro
    ESP_ERROR_CHECK(esp_netif_init());

    // 3. cria loop de eventos padrao
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 4. cria a interface padrao
    esp_netif_create_default_wifi_sta();

    // 5. inicializa o wifi com as configurações padrao
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
}

void wifi_manager_connect(void) {
    // registra o manipulador de eventos para eventos de wifi e ip
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    // configura as credenciais do wifi
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, WIFI_PASSWORD, sizeof(wifi_config.sta.password));

    // define o modo do wifi como estação (cliente)
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    
    // inicia o wifi
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG_WIFI, "WiFi inicializado com SSID:%s", WIFI_SSID);
    
    // espera até que esteja conectado ao wifi
    while (!wifi_connected) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
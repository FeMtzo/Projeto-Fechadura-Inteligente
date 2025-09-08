#include "security_manager.h"
#include "utils_defs.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h> // Para verificar se o arquivo existe

// Inclui a biblioteca do SPIFFS
#include "esp_spiffs.h"

// Define os nomes dos nossos arquivos no "banco de dados"
#define MASTER_TAG_FILE "/spiffs/master.txt"
#define RFID_TAGS_FILE  "/spiffs/tags.txt"

// A tag mestra agora é carregada para a memória RAM na inicialização
static uint8_t loaded_master_uid[8];
static uint8_t loaded_master_uid_len = 0;

// Função auxiliar para converter um UID de bytes para uma string hexadecimal
static void uid_to_hex_string(const uint8_t* uid, uint8_t len, char* str_buffer)
{
    for (int i = 0; i < len; i++) {
        sprintf(str_buffer + (i * 2), "%02X", uid[i]);
    }
}

esp_err_t security_manager_init(void)
{
    ESP_LOGI(TAG_SECURITY, "Inicializando o sistema de arquivos SPIFFS...");
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = "storage",
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG_SECURITY, "Falha ao montar ou formatar o filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG_SECURITY, "Particao SPIFFS nao encontrada");
        } else {
            ESP_LOGE(TAG_SECURITY, "Falha ao inicializar SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    
    // Tenta carregar a Tag Mestra do arquivo
    FILE* f = fopen(MASTER_TAG_FILE, "r");
    if (f == NULL) {
        ESP_LOGW(TAG_SECURITY, "Arquivo da Tag Mestra nao encontrado. O sistema precisa ser provisionado.");
        return ESP_ERR_NOT_FOUND; // Sinaliza que é a primeira inicialização
    }

    char uid_str[17] = {0}; // 8 bytes = 16 chars hex + \0
    fgets(uid_str, sizeof(uid_str), f);
    fclose(f);

    // Converte de string para bytes (lógica inversa do uid_to_hex_string)
    loaded_master_uid_len = strlen(uid_str) / 2;
    for (int i = 0; i < loaded_master_uid_len; i++) {
        sscanf(uid_str + 2 * i, "%2hhx", &loaded_master_uid[i]);
    }

    ESP_LOGI(TAG_SECURITY, "Tag Mestra carregada do arquivo.");
    ESP_LOG_BUFFER_HEX(TAG_SECURITY, loaded_master_uid, loaded_master_uid_len);
    return ESP_OK;
}

esp_err_t security_manager_set_master_tag(const uint8_t* uid, uint8_t len)
{
    FILE* f = fopen(MASTER_TAG_FILE, "w");
    if (f == NULL) {
        ESP_LOGE(TAG_SECURITY, "Falha ao abrir o arquivo da Tag Mestra para escrita.");
        return ESP_FAIL;
    }
    char uid_str[len * 2 + 1] = {0};
    uid_to_hex_string(uid, len, uid_str);
    fprintf(f, "%s", uid_str);
    fclose(f);
    ESP_LOGW(TAG_SECURITY, "Nova Tag Mestra salva no arquivo com sucesso!");
    return ESP_OK;
}

bool security_manager_is_master_tag(const uint8_t* uid, uint8_t len)
{
    if (loaded_master_uid_len == 0 || len != loaded_master_uid_len) {
        return false;
    }
    return (memcmp(uid, loaded_master_uid, len) == 0);
}

bool security_manager_verify_rfid(const uint8_t* uid, uint8_t len)
{
    FILE* f = fopen(RFID_TAGS_FILE, "r");
    if (f == NULL) {
        ESP_LOGW(TAG_SECURITY, "Arquivo de tags nao existe. Nenhuma tag eh valida.");
        return false; // Se o arquivo não existe, nenhuma tag pode ser válida
    }

    char uid_to_check_str[len * 2 + 1] = {0};
    uid_to_hex_string(uid, len, uid_to_check_str);

    char line[32];
    bool found = false;
    while (fgets(line, sizeof(line), f)) {
        // Remove o caractere de nova linha '\n' se existir
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, uid_to_check_str) == 0) {
            found = true;
            break;
        }
    }
    fclose(f);
    return found;
}

esp_err_t security_manager_add_rfid(const uint8_t* uid, uint8_t len)
{
    if (security_manager_verify_rfid(uid, len)) {
        ESP_LOGW(TAG_SECURITY, "Tag ja cadastrada. Nao foi adicionada novamente.");
        return ESP_OK; // Já existe, então consideramos sucesso
    }

    // "a" significa "append" (adicionar ao final do arquivo)
    FILE* f = fopen(RFID_TAGS_FILE, "a");
    if (f == NULL) {
        ESP_LOGE(TAG_SECURITY, "Falha ao abrir arquivo de tags para adicionar.");
        return ESP_FAIL;
    }
    char uid_str[len * 2 + 1] = {0};
    uid_to_hex_string(uid, len, uid_str);
    fprintf(f, "%s\n", uid_str);
    fclose(f);
    ESP_LOGI(TAG_SECURITY, "Tag %s adicionada com sucesso.", uid_str);
    return ESP_OK;
}

void security_manager_list_all_rfid_tags(void)
{
    FILE* f = fopen(RFID_TAGS_FILE, "r");
    if (f == NULL) {
        ESP_LOGI(TAG_SECURITY, "--- Nenhuma Tag RFID Cadastrada (arquivo nao encontrado) ---");
        return;
    }
    
    ESP_LOGI(TAG_SECURITY, "--- Lista de Tags RFID Cadastradas ---");
    char line[32];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // Remove a nova linha
        ESP_LOGI(TAG_SECURITY, "Tag [%d]: %s", count + 1, line);
        count++;
    }
    if(count == 0) {
        ESP_LOGI(TAG_SECURITY, "Nenhuma tag cadastrada no arquivo.");
    }
    ESP_LOGI(TAG_SECURITY, "------------------------------------");
    fclose(f);
}

// A função de remover é a mais complexa com arquivos de texto
esp_err_t security_manager_remove_rfid(const uint8_t* uid, uint8_t len)
{
    char uid_to_remove_str[len * 2 + 1] = {0};
    uid_to_hex_string(uid, len, uid_to_remove_str);
    
    // 1. Abre o arquivo original para leitura e um temporário para escrita
    FILE* f_read = fopen(RFID_TAGS_FILE, "r");
    if(!f_read) return ESP_FAIL; // Arquivo original não existe

    char temp_file_path[] = "/spiffs/temp.txt";
    FILE* f_write = fopen(temp_file_path, "w");
    if(!f_write) {
        fclose(f_read);
        return ESP_FAIL;
    }
    
    // 2. Copia todas as linhas, exceto a que queremos remover
    char line[32];
    bool removed = false;
    while(fgets(line, sizeof(line), f_read)) {
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line, uid_to_remove_str) != 0) {
            fprintf(f_write, "%s\n", line);
        } else {
            removed = true;
        }
    }
    fclose(f_read);
    fclose(f_write);

    if(removed) {
        // 3. Deleta o arquivo original
        remove(RFID_TAGS_FILE);
        // 4. Renomeia o arquivo temporário para o nome do original
        rename(temp_file_path, RFID_TAGS_FILE);
        ESP_LOGI(TAG_SECURITY, "Tag %s removida com sucesso.", uid_to_remove_str);
    } else {
        remove(temp_file_path); // A tag não foi encontrada, apaga o arquivo temp
        ESP_LOGW(TAG_SECURITY, "Tag %s nao encontrada para remocao.", uid_to_remove_str);
    }
    
    return ESP_OK;
}
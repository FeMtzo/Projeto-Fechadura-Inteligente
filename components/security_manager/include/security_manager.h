#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

/**
 * @brief Inicializa o gerenciador de segurança e o sistema NVS.
 * Tenta carregar a Tag Mestra da memória.
 *
 * @return ESP_OK se a tag mestra foi carregada, ESP_ERR_NVS_NOT_FOUND se nenhuma foi encontrada.
 */
esp_err_t security_manager_init(void);

/**
 * @brief Define e salva uma nova Tag Mestra no NVS.
 * @param uid Ponteiro para o array de bytes do UID da nova tag mestra.
 * @param len Comprimento do UID em bytes.
 * @return ESP_OK se sucesso.
 */
esp_err_t security_manager_set_master_tag(const uint8_t* uid, uint8_t len);

/**
 * @brief Verifica se um UID de tag RFID corresponde à tag mestra.
 * @param uid Ponteiro para o array de bytes do UID.
 * @param len Comprimento do UID em bytes.
 * @return true se o UID for da tag mestra, false caso contrário.
 */
bool security_manager_is_master_tag(const uint8_t* uid, uint8_t len);

/**
 * @brief Adiciona um novo UID de tag RFID ao banco de dados NVS.
 * @param uid Ponteiro para o array de bytes do UID.
 * @param len Comprimento do UID em bytes.
 * @return ESP_OK se sucesso.
 */
esp_err_t security_manager_add_rfid(const uint8_t* uid, uint8_t len);

/**
 * @brief Remove um UID de tag RFID do banco de dados NVS.
 * @param uid Ponteiro para o array de bytes do UID.
 * @param len Comprimento do UID em bytes.
 * @return ESP_OK se a tag foi removida com sucesso.
 */
esp_err_t security_manager_remove_rfid(const uint8_t* uid, uint8_t len);

/**
 * @brief Verifica se um UID de tag RFID existe no banco de dados NVS.
 * @param uid Ponteiro para o array de bytes do UID.
 * @param len Comprimento do UID em bytes.
 * @return true se o UID for válido, false caso contrário.
 */
bool security_manager_verify_rfid(const uint8_t* uid, uint8_t len);

/**
 * @brief Lista todos os UIDs de tags RFID cadastrados no NVS.
 *
 * A lista é impressa no Monitor Serial.
 */
void security_manager_list_all_rfid_tags(void);
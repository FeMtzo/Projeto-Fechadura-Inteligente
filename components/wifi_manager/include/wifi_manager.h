#pragma once
#include <esp_err.h>

/**
 * @brief inicializa o wifi
 * 
 */
void wifi_manager_init(void);

/**
 * @brief conecta ao wifi
 * 
 */
void wifi_manager_connect(void);
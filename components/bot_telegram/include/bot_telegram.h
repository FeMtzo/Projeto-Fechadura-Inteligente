#pragma once

/**
 * @brief inicializa o bot do telegram
 * 
 */
void bot_telegram_init(void);

/**
 * @brief envia uma mensagem via telegram
 * 
 * @param message mensagem a ser enviada
 */
void bot_telegram_send_message(const char* message);

/**
 * @brief verifica se ha novas mensagens no bot do telegram
 * 
 */
void bot_telegram_check_messages(void);

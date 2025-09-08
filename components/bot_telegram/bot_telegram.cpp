// // Incluindo bibliotecas necessárias
// #include <UniversalTelegramBot.h>

// // Definições do bot do Telegram
// #define BOT_TOKEN   "8225811146:AAFqUO2Nyxz9FfG-9JJW98t--I8sDr_kZNA"
// #define CHAT_ID     "6183959627"

// UniversalTelegramBot bot(BOT_TOKEN, secured_client);
// bool flag = false;

#include "bot_telegram.h"
#include "utils_defs.h"
#include "esp_log.h"

void telegram_bot_init(void)
{
    // Por enquanto, esta função apenas imprime uma mensagem para sabermos que foi chamada.
    // A lógica do bot virá aqui no futuro.
    ESP_LOGI(TAG_TELEGRAM, "Componente do Bot inicializado (logica a ser implementada).");
}
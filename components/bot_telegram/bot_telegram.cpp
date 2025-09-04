// Incluindo bibliotecas necessárias
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <arduino.h>

// Definições do bot do Telegram
#define BOT_TOKEN   "8225811146:AAFqUO2Nyxz9FfG-9JJW98t--I8sDr_kZNA"
#define CHAT_ID     "6183959627"

UniversalTelegramBot bot(BOT_TOKEN, secured_client);
bool flag = false;
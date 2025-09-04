idf_component_register(SRCS "bot_telegram.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES wifi_manager lock_actuator esp_http_client cjson) 



idf_component_register(SRCS "wifi_manager.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES utils esp_wifi esp_event esp_netif nvs_flash)
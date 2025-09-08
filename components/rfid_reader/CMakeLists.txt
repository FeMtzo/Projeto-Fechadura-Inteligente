idf_component_register(SRCS "rfid_reader.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES utils rc522 esp_event esp_driver_spi)
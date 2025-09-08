idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES rfid_reader security_manager ui_feedback lock_actuator nvs_flash)
idf_component_register(SRCS "main.c" "wifi.c"
                    INCLUDE_DIRS ".")
                    
spiffs_create_partition_image(storage ../webserver_files FLASH_IN_PROJECT)
                    
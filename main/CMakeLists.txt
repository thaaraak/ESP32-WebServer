idf_component_register(SRCS "main.cpp" "wifi.c" "webserver.c" "si5351.cpp" "fir_filter.c"
				"PhaseFilter.cpp" "streaming_http_audio.c"
                    INCLUDE_DIRS ".")
        
                    
spiffs_create_partition_image(storage ../webserver_files FLASH_IN_PROJECT)
                    
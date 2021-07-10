# Web Server for ESP32 Radio

### This is the web server component for the ESP32 IoT Radio consisting of
    
* Webserver with hostname set to configuration
    
* URI /command for changing the si5351 frequency
    
* SPIFFS partition for storing the static content
    
* React.js frequency control UI (see [ESP32 Radio Javascript code](http://github.com/thaaraak/ESP32-Radio-js) )
    
* Code for controlling the si5351 frequency synthesizer


### Configuration (using 'idf.py menuconfig'):

* Set the Partition table to Custom Partition Table CSV with file name 'partitions.csv'

* Set the SPIFFS maximum name length to 64

* Edit the SSID and password for your wifi
    
* Edit the hostname
    
* Build using 'idf.py build'

### ADF Changes

In order to use the "Generic ESP32" board the files from (https://github.com/thaaraak/ESP32-IQ/tree/main/adf-changes) need to be copied into your ESP-ADF/components/audio_board directory. These attached files create a simple "Generic ESP32" board in the menuconfig and allow for running the ADF on a standard ESP32. Note the pin configuration for the I2S pins can be found in 'board_pins_config.c'. The WS, BCLK and Data In/Out pins can be changed to any valid pin. The MCLK pin has to be one of GPIO 0, 1 or 3. Pin 1 is the UART TX so thats not ideal, Pin 0 is not exposed on the board I was using so I chosen Pin 3

Simply copy the files to the esp-adf/components/audio_board directory and re-run 'idf.py menuconfig'

# Web Server for ESP32 Radio

### This is the web server component for the ESP32 IoT Radio consisting of
    
* Webserver with hostname set to configuration
    
* URI /command for changing the si5351 frequency
    
* SPIFFS partition for storing the static content
    
* React.js frequency control UI
    
* Code for controlling the si5351 frequency synthesizer


### Configuration (using 'idf.py menuconfig'):

    1. Set the Partition table to Custom Partition Table CSV with file name 'partitions.csv'

    2. Set the SPIFFS maximum name length to 64

    3. Edit the SSID and password for your wifi
    
    4. Edit the hostname
    
    5. Build using 'idf.py build'


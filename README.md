# Web Server for ESP32 Radio

This is the web server component for the ESP32 IoT Radio consisting of
    
    1. Webserver with hostname set to configuration
    
    2. URI /command for changing the si5351 frequency
    
    3. SPIFFS partition for storing the static content
    
    4. React.js frequency control UI


Configuration:
    1. Set the Partition table to Custom Partition Table CSV with file name 'partitions.csv'

    2. Set the SPIFFS maximum name length to 64

    3. Edit the SSID and password for your wifi


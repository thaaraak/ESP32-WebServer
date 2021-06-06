# Web Server for ESP32 Radio

This is the web server component for the ESP32 IoT Radio consisting of
    1. Webserver with hostname set to configuration
    2. URI /command for changing the si5351 frequency
    3. SPIFFS partition for storing the static content
    4. React.js frequency control UI

* Set the Partition table to Custom Partition Table CSV with file name 'partitions.csv'
* Set the SPIFFS maximum name length to 64
* Edit the SSID and password for your wifi


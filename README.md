# Web Server for ESP32 Radio

### This is the web server component for the ESP32 IoT Radio consisting of
    
* Webserver with hostname set to configuration
    
* URI /command for changing the si5351 frequency
    
* SPIFFS partition for storing the static content
    
* React.js frequency control UI (see [ESP32 Radio Javascript code](http://github.com/thaaraak/ESP32-Radio-js)
    
* Code for controlling the si5351 frequency synthesizer


### Configuration (using 'idf.py menuconfig'):

* Set the Partition table to Custom Partition Table CSV with file name 'partitions.csv'

* Set the SPIFFS maximum name length to 64

* Edit the SSID and password for your wifi
    
* Edit the hostname
    
* Build using 'idf.py build'


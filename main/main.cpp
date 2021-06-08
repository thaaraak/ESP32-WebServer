#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/i2c.h"

#include "si5351.h"
#include "wifi.h"
#include "webserver.h"

#define I2C_MASTER_NUM	0
#define I2C_MASTER_SDA_IO 26
#define I2C_MASTER_SCL_IO 27

#define BASE_PATH "/spiffs"
static const char *TAG = "esp32-webserver";

extern "C" {
	void app_main();
	void command_callback( const char* command );
	void audio_process(void);

}

Si5351 synth;


static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

int lastMult = -1;

void changeFrequency( int currentFrequency )
{
	  int mult = 0;

	  if ( currentFrequency < 8000000 )
		  mult = 100;
	  else if ( currentFrequency < 11000000 )
		  mult = 80;
	  else if ( currentFrequency < 15000000 )
		  mult = 50;
	  else if ( currentFrequency < 22000000 )
		  mult = 40;
	  else if ( currentFrequency < 30000000 )
		  mult = 30;

	  uint64_t freq = currentFrequency * 100ULL;
	  uint64_t pllFreq = freq * mult;

	  synth.set_freq_manual(freq, pllFreq, SI5351_CLK0);
	  synth.set_freq_manual(freq, pllFreq, SI5351_CLK2);

	  if ( mult != lastMult )
	  {
		  synth.set_phase(SI5351_CLK0, 0);
		  synth.set_phase(SI5351_CLK2, mult);
		  synth.pll_reset(SI5351_PLLA);
		  lastMult = mult;
	  }
}

void command_callback( const char* command, char* response )
{

	ESP_LOGI( TAG, "In command callback: %s\n", command );

	char status[10];
	char message[32];

	strcpy( status, "FAIL" );
	strcpy( message, "Unknown Command" );

    char param[32];
    /* Get value of expected key from query string */
    if (httpd_query_key_value(command, "frequency", param, sizeof(param)) == ESP_OK) {
        float frequency = atof(param) * 1000000;
        ESP_LOGI(TAG, "Found URL query parameter => frequency=%08f", frequency);

        if ( frequency >= 7000000 && frequency <= 30000000 ) {
        	strcpy( status, "OK" );
        	sprintf( message, "Frequency changed to %08f", frequency );
        	changeFrequency( frequency );
        }

        else {
        	sprintf( message, "Invalid Frequency" );
        }
    }

    sprintf( response, "{ \"status\" : \"%s\", \"message\" : \"%s\" }", status, message );


}

static esp_err_t init_spiffs(const char *base_path)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = base_path,
      .partition_label = NULL,
      .max_files = 5,   // This decides the maximum number of files that can be created on the storage
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}


void init_webserver(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    init_spiffs( BASE_PATH );
    wifi_connect_with_hostname( CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD, CONFIG_ESP_HOSTNAME );
    start_webserver( BASE_PATH, command_callback );
}


void app_main()
{
    ESP_LOGI(TAG, "Initializing Web Server");
	init_webserver();

    ESP_LOGI(TAG, "Initializing I2C");
	i2c_master_init();

	ESP_LOGI(TAG, "Initializing si5351");
	synth.init( I2C_MASTER_NUM, SI5351_CRYSTAL_LOAD_8PF, 25000000, 0 );

	int freq = 14200000;
	changeFrequency(freq);

	ESP_LOGI(TAG, "Entering Audio Processing loop");
	audio_process();

    while(1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);

    }
}

/*
 * PhaseFilter.cpp
 *
 *  Created on: Jun 10, 2021
 *      Author: xenir
 */
#include "PhaseFilter.h"

static const char *TAG = "PhaseFilter";


PhaseFilter::PhaseFilter( int sample_rate, i2s_bits_per_sample_t bits, int len, float* left, float* right )
{
	_sample_rate = sample_rate;
	_bits = bits;

	_len = len;
	_left = left;
	_right = right;
}

void PhaseFilter::initFIRConfig( fir_filter_cfg_t& cfg )
{
	memset( &cfg, 0, sizeof(fir_filter_cfg_t));

    cfg.out_rb_size        = fir_filter_RINGBUFFER_SIZE;
    cfg.task_stack         = fir_filter_TASK_STACK;
    cfg.task_core          = fir_filter_TASK_CORE;
    cfg.task_prio          = fir_filter_TASK_PRIO;
    cfg.stack_in_ext       = true;

    cfg.firLen = _len;
    cfg.coeffsLeft = _left;
    cfg.coeffsRight = _right;

}

void PhaseFilter::initI2SConfig( i2s_stream_cfg_t& cfg, audio_stream_type_t t )
{
	memset( &cfg, 0, sizeof(i2s_stream_cfg_t));

    cfg.type = t;

	cfg.i2s_config.mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX);
	cfg.i2s_config.sample_rate = _sample_rate;
	cfg.i2s_config.bits_per_sample = _bits;
	cfg.i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
	cfg.i2s_config.communication_format = I2S_COMM_FORMAT_I2S;
	cfg.i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL2 | ESP_INTR_FLAG_IRAM;
	cfg.i2s_config.dma_buf_count = 3;
	cfg.i2s_config.dma_buf_len = 300;
	cfg.i2s_config.use_apll = true;
	cfg.i2s_config.tx_desc_auto_clear = true;
	cfg.i2s_config.fixed_mclk = 0;

	cfg.i2s_port = I2S_NUM_0;
    cfg.use_alc = false;
    cfg.volume = 0;
    cfg.out_rb_size = I2S_STREAM_RINGBUFFER_SIZE;
    cfg.task_stack = I2S_STREAM_TASK_STACK;
    cfg.task_core = I2S_STREAM_TASK_CORE;
    cfg.task_prio = I2S_STREAM_TASK_PRIO;
    cfg.stack_in_ext = false;
    cfg.multi_out_num = 0;
    cfg.uninstall_drv = true;

}

void PhaseFilter::setGain( int gain )
{
	fir_filter_set_gain( fir_filter, gain );
}

void PhaseFilter::setSideband( phase_filter_sideband_t sideband )
{
	if ( sideband == UPPER_SIDEBAND )
		fir_filter_set_operand( fir_filter, 1 );
	else
		fir_filter_set_operand( fir_filter, -1 );

}

void PhaseFilter::init()
{

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "[ 1 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();

    // Set codec mode to "BOTH" to enabled ADC and DAC. Coded Mode line in simply
    // adds the Line In2 to the DAC bypassing the ADC

    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[ 2 ] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[3.1] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg;
    initI2SConfig( i2s_cfg, AUDIO_STREAM_WRITER );
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[3.2] Create FIR Filter");
    fir_filter_cfg_t fir_filter_cfg;
    initFIRConfig( fir_filter_cfg );
    fir_filter = fir_filter_init(&fir_filter_cfg);

    ESP_LOGI(TAG, "[3.3] Create i2s stream to read data from codec chip");
    i2s_stream_cfg_t i2s_cfg_read;
    initI2SConfig( i2s_cfg_read, AUDIO_STREAM_READER );
    i2s_stream_reader = i2s_stream_init(&i2s_cfg_read);

    ESP_LOGI(TAG, "[3.3] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, i2s_stream_reader, "i2s_read");
    audio_pipeline_register(pipeline, fir_filter, "fir");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s_write");

    ESP_LOGI(TAG, "[3.4] Link it together [codec_chip]-->i2s_stream_reader-->i2s_stream_writer-->[codec_chip]");

    const char *link_tag[3] = {"i2s_read", "fir", "i2s_write"};
    audio_pipeline_link(pipeline, &link_tag[0], 3);

    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

}

void PhaseFilter::run()
{
    ESP_LOGI(TAG, "[ 5 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);

    ESP_LOGI(TAG, "[ 6 ] Listen for all pipeline events");

    while (1) {
        audio_event_iface_msg_t msg;

        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
            && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    ESP_LOGI(TAG, "[ 7 ] Stop audio_pipeline");
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, i2s_stream_reader);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);
    audio_pipeline_unregister(pipeline, fir_filter);

    /* Terminate the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(i2s_stream_reader);
    audio_element_deinit(i2s_stream_writer);
    audio_element_deinit(fir_filter);

}






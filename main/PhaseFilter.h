/*
 * PhaseFilter.h
 *
 *  Created on: Jun 10, 2021
 *      Author: xenir
 */

#ifndef MAIN_PHASEFILTER_H_
#define MAIN_PHASEFILTER_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "audio_pipeline.h"
#include "i2s_stream.h"
#include "board.h"
#include "fir_filter.h"
#include "streaming_http_audio.h"

typedef enum {
    UPPER_SIDEBAND    = 0,
    LOWER_SIDEBAND    = 1
} phase_filter_sideband_t;


class PhaseFilter
{
public:
	PhaseFilter( int sample_rate, i2s_bits_per_sample_t bits, int len, float* left, float* right, bool lineout );
	void init();
	void run();

	void setGain( int gain );
	void setSideband( phase_filter_sideband_t sideband );

private:
    audio_pipeline_handle_t 	pipeline;

    audio_element_handle_t 		i2s_stream_writer;
    audio_element_handle_t 		i2s_stream_reader;
    audio_element_handle_t 		fir_filter;
    audio_element_handle_t 		http_audio;

    audio_event_iface_handle_t 	evt;

    void initI2SConfig( i2s_stream_cfg_t& cfg, audio_stream_type_t t );
    void initFIRConfig( fir_filter_cfg_t& cfg );
    void initStreamingConfig( streaming_http_audio_cfg_t& cfg );

	int _sample_rate;
	i2s_bits_per_sample_t _bits;

	int		_len;
	float*	_left;
	float*	_right;
	bool	_audio_to_lineout;
};

#endif /* MAIN_PHASEFILTER_H_ */

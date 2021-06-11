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



class PhaseFilter
{
public:
	PhaseFilter( int sample_rate, i2s_bits_per_sample_t bits, int len, float* left, float* right );
	void init();
	void run();

private:
    audio_pipeline_handle_t 	pipeline;
    audio_element_handle_t 		i2s_stream_writer, i2s_stream_reader, fir_filter;
    audio_event_iface_handle_t 	evt;

    void initI2SConfig( i2s_stream_cfg_t& cfg, audio_stream_type_t t );
    void initFIRConfig( fir_filter_cfg_t& cfg );

	int _sample_rate;
	i2s_bits_per_sample_t _bits;

	int		_len;
	float*	_left;
	float*	_right;
};

#endif /* MAIN_PHASEFILTER_H_ */

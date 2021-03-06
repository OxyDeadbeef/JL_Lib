/* Lib Aldaron --- Copyright (c) 2016 Jeron A. Lau */
/* This file must be distributed with the GNU LESSER GENERAL PUBLIC LICENSE. */
/* DO NOT REMOVE THIS NOTICE */

#ifndef LA_AUDIO
#define LA_AUDIO

#include <la_config.h>
#ifndef LA_FEATURE_AUDIO
	#error "please add #define LA_FEATURE_AUDIO to your la_config.h"
#endif

#include <la_buffer.h>
#include <la_math.h>

// Types:
typedef struct {
	void *audio;
	int32_t channel;
	float seconds_in; // Where Music Should Start - not used yet
}la_sound_t;

// Prototypes:
void la_audio_play(la_sound_t* audio, float in, la_v3_t* vec);
void la_audio_lock(la_sound_t* audio, float in, la_v3_t* vec);
void la_audio_pause(la_sound_t*);
void la_audio_resume(la_sound_t*);
uint8_t la_audio_wait(la_sound_t*, la_sound_t*, float, la_v3_t*);
void la_audio_stop(la_sound_t*, float);
void la_audio_load(la_sound_t*, la_buffer_t*, const char*, uint8_t);

#endif

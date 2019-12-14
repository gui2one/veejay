#ifndef WAVE_FILE_READER_H
#define WAVE_FILE_READER_H

#include "pch.h"
#include <sndfile.h>
#include <math.h>
#include "portaudio.h"

#define PI (3.141592653)

enum SOUND_PLAYER_CMD{
	SOUND_PLAYER_CMD_PLAY,
	SOUND_PLAYER_CMD_STOP,
	SOUND_PLAYER_CMD_PAUSE
};

enum SOUND_PLAYER_MODE{
	SOUND_PLAYER_MODE_FILE,
	SOUND_PLAYER_MODE_SINE_WAVE
};

#define FRAMES_PER_BUFFER   (512)



struct callback_data_s
{
    SNDFILE     *file;
    SF_INFO      info;
    bool 		 loop;
    float * sound_buffer;
    SOUND_PLAYER_MODE * sound_player_mode;
    SOUND_PLAYER_CMD * sound_player_cmd;
    float * sine_wave;
    float * sine_wave_frequency;
    int sine_wave_offset = 0;
    
};

static int callback(    const void *input, void *output,
						unsigned long frameCount,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags,
						void *userData );	

class WaveFileReader
{
	public:
		WaveFileReader(float  * sound_buffer);
		int read(const char * path);
		void play(SOUND_PLAYER_CMD * cmd, SOUND_PLAYER_MODE * mode, float * sine_wave_frequency, bool loop = false);
		//~ double * make_sine_wave(float freq = 440.0f); 
	
		
		SNDFILE *file;
		PaStream *stream;
		PaError error;
		callback_data_s data;
		float * sound_buffer;
		
		float sine_wave[512 * 2];
	private:
		/* add your private declarations */
};




#endif /* WAVE_FILE_READER_H */ 

#include "wave_file_reader.h"


WaveFileReader::WaveFileReader(float * _sound_buffer)
{
	sound_buffer = _sound_buffer;
	for(size_t i = 0; i < FRAMES_PER_BUFFER * 2; i++)
	{
		sine_wave[i] = (double)(sin( (double)(i)* PI * 2.0 * 0.01   )) * 0.0005;
	}	
}

int WaveFileReader::read(const char * path)
{
    /* Open the soundfile */
    data.file = sf_open(path, SFM_READ, &data.info);
    data.sound_buffer = sound_buffer;
    if (sf_error(data.file) != SF_ERR_NO_ERROR)
    {
        fprintf(stderr, "%s\n", sf_strerror(data.file));
        fprintf(stderr, "File: %s\n", path);
        return 1;
    }
    
    /* init portaudio */
    //// temporarely redirect Port audio init messages to /dev/null to avoid ugly start
    freopen("/dev/null", "w", stderr);
    error = Pa_Initialize();
    freopen("/dev/tty", "w", stderr);
    
    if(error != paNoError)
    {
        fprintf(stderr, "Problem initializing\n");
        return 1;
    }
    
    /* Open PaStream with values read from the file */
    error = Pa_OpenDefaultStream(&stream
                                 ,0                     /* no input */
                                 ,data.info.channels         /* stereo out */
                                 ,paFloat32             /* floating point */
                                 ,data.info.samplerate
                                 ,FRAMES_PER_BUFFER
                                 ,callback
                                 ,&data);        /* our sndfile data struct */
    if(error != paNoError)
    {
        fprintf(stderr, "Problem opening Default Stream\n");
        return 1;
    }
    

    
    return 0;
		
}

void WaveFileReader::play(SOUND_PLAYER_CMD * cmd, SOUND_PLAYER_MODE * mode, float * sine_wave_frequency, bool loop){
    /* Start the stream */
    data.loop = loop;
    data.sound_player_cmd = cmd;
    data.sound_player_mode = mode;
    data.sine_wave = sine_wave;
    data.sine_wave_offset = 0;
    data.sine_wave_frequency = sine_wave_frequency;
    error = Pa_StartStream(stream);
    if(error != paNoError && *cmd != SOUND_PLAYER_CMD_STOP)
    {
        fprintf(stderr, "Problem Starting Stream\n");        
    }

    /* Run until EOF is reached */
    while(Pa_IsStreamActive(stream))
    {
		if( *cmd == SOUND_PLAYER_CMD_STOP)
			break;
        Pa_Sleep(100);
    }
    
    //~ printf("---------------\n");
    //~ printf("-----Closing PortAudio stream\n");
    //~ printf("---------------\n");
    Pa_CloseStream( stream );
    Pa_Terminate();
}



static int callback
    (const void                     *input
    ,void                           *output
    ,unsigned long                   frameCount
    ,const PaStreamCallbackTimeInfo *timeInfo
    ,PaStreamCallbackFlags           statusFlags
    ,void                           *userData
    )
{
    float           *out;
    callback_data_s *p_data = (callback_data_s*)userData;
    sf_count_t       num_read;

    out = (float*)output;
    p_data = (callback_data_s*)userData;

    /* clear output buffer */
    memset(out, 0, sizeof(float) * frameCount * p_data->info.channels);
	
    
    // copy data into main soud buffer
    if( *(p_data->sound_player_cmd) == SOUND_PLAYER_CMD_PLAY)
    {
	if( *(p_data->sound_player_mode) == SOUND_PLAYER_MODE_FILE)
	{
	    /* read directly into output buffer */
    
	    num_read = sf_read_float(p_data->file, out, frameCount * p_data->info.channels);		
	    memcpy(p_data->sound_buffer, out, sizeof(float) * frameCount * p_data->info.channels);
		
	}else if( *(p_data->sound_player_mode) == SOUND_PLAYER_MODE_SINE_WAVE){		
		
	    float ratio = ( frameCount / 44100.0) / (1.0/ *(p_data->sine_wave_frequency)) ;

	    for(size_t i = 0; i < frameCount * 2; i++)
	    {
		    float wave_val = (sin( ((float)(i + p_data->sine_wave_offset) / (float)frameCount * PI * 1.0) * ratio )) * 0.05;
		    *out++ = (float)wave_val;
		    if( i % 2 == 0)
			    p_data->sound_buffer[i/2] = wave_val;
	    }	
	    
	    p_data->sine_wave_offset += frameCount * p_data->info.channels;
		

	}
		
    }else if( *(p_data->sound_player_cmd) == SOUND_PLAYER_CMD_PAUSE){
				
	memset(p_data->sound_buffer, 0, sizeof(float) * frameCount * p_data->info.channels);		
    }
    	

    
    
    /*  If we couldn't read a full frameCount of samples we've reached EOF */
    if (num_read < frameCount && p_data->loop)
    {
	sf_seek(p_data->file, 0, SEEK_SET);
        
    }else if(num_read < frameCount){
	
	return paComplete;
    }
    
    return paContinue;
}




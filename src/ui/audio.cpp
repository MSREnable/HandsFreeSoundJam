#include "RtAudio.h"

static RtAudio global_audio;
extern "C" {
#include <soundpipe.h>
#include <stdlib.h>
#include "dsp/dsp.h"

struct jam_audio {
    int sr;
    RtAudio *audio;
};


static int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data )
{
    SPFLOAT * output = (SPFLOAT *)outputBuffer;
    int i;
    jam_audio *jam = (jam_audio *)data;

    for(i = 0; i < numFrames * 2; i+=2) {
        whisper_eyejam_tick_stereo(&output[i], &output[i + 1]);
    }
    
    return 0;
}

void jam_audio_init(jam_audio *jam)
{
    unsigned int buffer_frames = 1024;
    RtAudio *audio = jam->audio;
   
    RtAudio::DeviceInfo info; 
    RtAudio::StreamParameters iParams, oParams;
    info = audio->getDeviceInfo(audio->getDefaultOutputDevice());
    iParams.deviceId = audio->getDefaultInputDevice();
    iParams.nChannels = 0;
    iParams.firstChannel = 1;
    oParams.deviceId = audio->getDefaultOutputDevice();
    oParams.nChannels = 2;
    oParams.firstChannel = 0;
    
    RtAudio::StreamOptions options;
    
  
    audio->openStream( &oParams, NULL, 
            RTAUDIO_FLOAT32, info.preferredSampleRate, 
            &buffer_frames, &callme, jam, &options);
    audio->showWarnings( true );

    jam->sr = info.preferredSampleRate;
    whisper_eyejam_init(info.preferredSampleRate);
    /* whisper_eyejam_stress_test(); */
    /* whisper_eyejam_demo_clips(); */
    whisper_eyejam_db_open("data.db");
    whisper_eyejam_db_load_default();
    /* whisper_tracks_loopmode(0, 1); */

    /* set eyejam tempo */
    /* whisper_eyejam_tempo_set(125); */
}

void jam_audio_start(jam_audio *jam)
{
    jam->audio->startStream();
}

void jam_audio_stop(jam_audio *jam)
{
    jam->audio->stopStream();
}

void jam_audio_create(jam_audio **jam)
{
    *jam = (jam_audio *)malloc(sizeof(jam_audio));
    (*jam)->audio = &global_audio;
}

void jam_audio_destroy(jam_audio **jam)
{
    whisper_eyejam_db_save_default();
    whisper_eyejam_db_close();
    whisper_eyejam_destroy();
    free(*jam);
}

int jam_audio_samplerate(jam_audio *jam)
{
    return jam->sr;
}

}

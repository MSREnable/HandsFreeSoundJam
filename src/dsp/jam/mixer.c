/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <math.h>
#include "dsp.h"
#include "moongazing/moongazing.h"
#include "arachnoid/arachnoid.h"
#define NSENDS 2
#define DB2LIN(A) pow(10, (double)(A)/20)

struct whisper_effect {
    whisper_effect_init init; 
    whisper_effect_compute compute; 
    whisper_effect_destroy destroy; 
    void *ud;
};

struct whisper_mixer {
    SPFLOAT send[WHISPER_NTRACKS][NSENDS];
    whisper_effect effect[NSENDS];
    SPFLOAT throw[NSENDS];
    mg_synth *moonjam;
};


/* global data */
static whisper_mixer mixer;


/* initalizes mixer: may call soundpipe */
void whisper_mixer_init(int sr)
{
    /* make sure whisper_mixer_setup() is called first */
    int e;
    for(e = 0; e < NSENDS; e++) {
        mixer.effect[e].init(&mixer.effect[e], sr);
    }
    mixer.moonjam = mg_synth_data();
    mg_synth_create(mixer.moonjam, sr);
    arachnoid_audio_init();
}

void whisper_mixer_destroy()
{
    int e;
    for(e = 0; e < NSENDS; e++) {
        mixer.effect[e].destroy(&mixer.effect[e]);
    }
    mg_synth_destroy(mixer.moonjam);
    arachnoid_audio_stop();
}

void whisper_mixer_setup()
{
    int track, send; 
    int e;

    /* zero out sends with this poor mans memset */

    for(track = 0; track < WHISPER_NTRACKS; track++) {
        for(send = 0; send < NSENDS; send++) {
           mixer.send[track][send] = 0.f; 
        }
    }

    /* set up effects */
    for(e = 0; e < NSENDS; e++) {
        whisper_effect_setup(&mixer.effect[e]);
        mixer.throw[e] = 0.f;
    }

    /* bind reverb unit to effect slot 0 */
    whisper_reverb_bind(&mixer.effect[0]);

    /* send a little bit of track 0 (melody) to reverb in slot 0 */
    whisper_mixer_send(0, 0, -12.f);
    
    /* send a little bit of track 3 (chords) to reverb in slot 0 */
    whisper_mixer_send(3, 0, -10.f);

    /* bind delay unit to effect slot 1 */
    whisper_delay_bind(&mixer.effect[1]);
    
    /* send a little bit of track 0 (melody) to delay in slot 1 */
    whisper_mixer_send(0, 1, -6.f);
    
    /* send a little bit of track 4 (alt melody) to delay in slot 1 */
    whisper_mixer_send(4, 1, -10.f);
    
    /* send a little bit of track 4 (alt melody) to reverb in slot 0 */
    whisper_mixer_send(4, 0, -10.f);

}


void whisper_mixer_tick(SPFLOAT *clock, SPFLOAT *L, SPFLOAT *R)
{
    /* simply compute the tracks for now */
    int t, s;
    SPFLOAT s_track;
    SPFLOAT send[NSENDS];
    SPFLOAT tmpL;
    SPFLOAT tmpR;
    
    *L = 0.f;
    *R = 0.f;

    for(s = 0; s < NSENDS; s++) {
        send[s] = 0.f; 
        mixer.throw[s] = 0.f;
    }

    for(t = 0; t < WHISPER_NTRACKS; t++ ) {
        whisper_tracks_tick_track(clock, &s_track, t);
        *L += s_track;
        *R += s_track;

        /* add to send channel */
        for(s = 0; s < NSENDS; s++) send[s] += s_track * mixer.send[t][s];
    }


    /* compute moonjam */

    mg_synth_tick(mixer.moonjam, &tmpL);
    *L += tmpL;
    *R += tmpL;

    /* compute arachnoid */

    tmpL = arachnoid_tick();
    *L += tmpL;
    *R += tmpL;

    /* compute global effects */

    for(s = 0; s < NSENDS; s++) {
        send[s] += mixer.throw[s];
        mixer.effect[s].compute(&mixer.effect[s], &send[s], &tmpL, &tmpR);
        *L += tmpL;
        *R += tmpR;
    }
}


void whisper_mixer_send(int track, int send, SPFLOAT amt)
{
    if(amt > -100.f)  {
        mixer.send[track][send] = DB2LIN(amt);
    } else {
        mixer.send[track][send] = 0.f;
    }
}

/* Effects */

/* Dummy functions */

static void compute_nothing(whisper_effect *e, SPFLOAT *a, SPFLOAT *L, SPFLOAT *R)
{ 
    /* set L + R outputs to be zero */
    *L = 0.f;
    *R = 0.f;
}

static void init_nothing(whisper_effect *e, int sr)
{ }

static void destroy_nothing(whisper_effect *e)
{ }

void whisper_effect_setup(whisper_effect *effect)
{
    whisper_effect_cb_init(effect, init_nothing);
    whisper_effect_cb_compute(effect, compute_nothing);
    whisper_effect_cb_destroy(effect, destroy_nothing);
}

void whisper_effect_cb_init(whisper_effect *effect, whisper_effect_init init)
{
    effect->init = init;
}

void whisper_effect_cb_destroy(whisper_effect *effect, whisper_effect_destroy destroy)
{
    effect->destroy = destroy;
}

void whisper_effect_cb_compute(whisper_effect *effect, whisper_effect_compute compute)
{
    effect->compute = compute;
}

void whisper_effect_data_set(whisper_effect *effect, void *ud)
{
    effect->ud = ud;
}

void * whisper_effect_data_get(whisper_effect *effect)
{
    return effect->ud;
}

void whisper_mixer_reset()
{
    /* reset delay line in effect slot 1 */
    whisper_delay_reset(&mixer.effect[1]);
}

void whisper_mixer_throw(int send, SPFLOAT val)
{
    mixer.throw[send] += val;
}

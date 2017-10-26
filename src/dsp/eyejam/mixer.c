#include <math.h>
#include "dsp.h"
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

}
void whisper_mixer_destroy()
{
    int e;
    for(e = 0; e < NSENDS; e++) {
        mixer.effect[e].destroy(&mixer.effect[e]);
    }
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

    for(s = 0; s < NSENDS; s++) send[s] = 0.f; 

    for(t = 0; t < WHISPER_NTRACKS; t++ ) {
        whisper_tracks_tick_track(clock, &s_track, t);
        *L += s_track;
        *R += s_track;

        /* add to send channel */
        for(s = 0; s < NSENDS; s++) send[s] += s_track * mixer.send[t][s];
    }

    /* compute global effects */

    for(s = 0; s < NSENDS; s++) {
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

/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include "dsp.h"
#include "loadflac.h"

#define DK_NVOICES 4
#define DK_NSAMPLES 8
#define DK_NOTEBIAS 60

typedef struct whisper_drumkit whisper_drumkit;
typedef struct drumkit_voice drumkit_voice;
typedef struct drumkit_sample drumkit_sample;

struct drumkit_sample {
    sp_ftbl ft;
    SPFLOAT dur;
    SPFLOAT gain;
}; 

struct drumkit_voice {
    sp_phasor *ph;
    sp_tabread *tr;
    sp_tgate *tg;
    int please_tick;
    int next_samp;
    SPFLOAT *gain;
    drumkit_sample *cur;
    uint32_t pos;
    int playing;
};


struct whisper_drumkit {
    sp_data *sp;
    SPFLOAT *silence;
    drumkit_sample sample[DK_NSAMPLES];
    drumkit_voice voice[DK_NVOICES];
    int kit;
};

static whisper_drumkit drumkit;

static void drumkit_voice_set_samp(drumkit_voice *voc, drumkit_sample *smp)
{
    voc->tg->time = smp->dur;
    voc->ph->freq = 1.f / smp->dur;
    voc->ph->curphs = 0.f;
    voc->tr->ft = &smp->ft;
    voc->gain = &smp->gain;
    voc->cur = smp;
    voc->pos = 0;
}

static void drumkit_voice_init(sp_data *sp, 
    whisper_drumkit *kit, drumkit_voice *voc, drumkit_sample *smp)
{
    voc->please_tick = 0;

    sp_tgate_create(&voc->tg);
    sp_tgate_init(sp, voc->tg);

    sp_phasor_create(&voc->ph);
    sp_phasor_init(sp, voc->ph, 0.f);


    sp_tabread_create(&voc->tr);
    sp_tabread_init(sp, voc->tr, &smp->ft, 1);

    drumkit_voice_set_samp(voc, smp);
    voc->next_samp = 0;
    voc->playing = 0;
}

static void drumkit_sample_silence(whisper_drumkit *k, drumkit_sample *s)
{
    sp_ftbl_init(k->sp, &s->ft, k->sp->sr * 0.5f);
    s->ft.del = 0;
    s->ft.tbl = k->silence;
}

static void drumkit_sample_destroy(drumkit_sample *samp);

static void drumkit_sample_load(whisper_drumkit *kit,
        drumkit_sample *smp, const char *filename)
{
    int r;
    /* TODO: make sure previously allocated samples get freed somehow */
    drumkit_sample_destroy(smp);
    r = sp_ftbl_loadflac(kit->sp, &smp->ft, filename);
    if(r == SP_NOT_OK) {
        drumkit_sample_silence(kit, smp);
    }
    smp->dur = (SPFLOAT)smp->ft.size / kit->sp->sr;
    smp->gain = 1.f;
}

static void drumkit_create(sp_data *sp, whisper_drumkit *kit)
{
    int v;
    kit->sp = sp;
    kit->silence = malloc(sizeof(SPFLOAT) * sp->sr * 0.5f);
  
    for(v = 0; v < DK_NSAMPLES; v++) {
        drumkit_sample_silence(kit, &kit->sample[v]);
    }

    /*
    drumkit_sample_load(kit, &kit->sample[0], "samples/simplekit/kick.flac");
    drumkit_sample_load(kit, &kit->sample[1], "samples/simplekit/snare.flac");
    drumkit_sample_load(kit, &kit->sample[2], "samples/simplekit/hh.flac");
    */

    /* turn down hihat */
    kit->sample[2].gain = 0.3f;
    
    /* turn down snare */
    kit->sample[1].gain = 0.5f;
    
    /* turn up kick */
    kit->sample[0].gain = 2.5f;

    for(v = 0; v < DK_NVOICES; v++) {
        drumkit_voice_init(sp, kit, &kit->voice[v], &kit->sample[0]);
    }
}

static void drumkit_voice_destroy(drumkit_voice *voc)
{
    sp_tgate_destroy(&voc->tg);
    sp_phasor_destroy(&voc->ph);
    sp_tabread_destroy(&voc->tr);
}

static void drumkit_sample_destroy(drumkit_sample *samp)
{
    if(samp->ft.del) {
        free(samp->ft.tbl);
    }
}

static void drumkit_destroy(whisper_drumkit *kit)
{
    int s;
    for(s = 0; s < DK_NVOICES; s++) 
        drumkit_voice_destroy(&kit->voice[s]);
    for(s = 0; s < DK_NSAMPLES; s++)
        drumkit_sample_destroy(&kit->sample[s]);

    free(kit->silence);
}

void whisper_drumkit_init(int sr)
{
    /*TODO: make centralized */
    sp_data *sp;

    sp_create(&sp);
    sp->sr = sr;

    drumkit_create(sp, &drumkit);
    
    drumkit.kit = -1;
}

void whisper_drumkit_destroy(void)
{
    /*TODO: make centralized */
    sp_data *sp;

    sp = drumkit.sp;

    drumkit_destroy(&drumkit);
    sp_destroy(&sp);
}

static void drumkit_voice_tick(whisper_drumkit *kit, 
        drumkit_voice *voc, SPFLOAT *sample)
{
    *sample = 0.f;

    if(voc->please_tick) {
        voc->please_tick = 0;
        voc->playing = 1;
        drumkit_voice_set_samp(voc, &kit->sample[voc->next_samp]); 
    }

    if(voc->playing) {
        if(voc->pos < voc->cur->ft.size) {
            *sample = voc->cur->ft.tbl[voc->pos] * *voc->gain ;
            voc->pos++;
        }else {
            voc->playing = 0;
        }
    }
}

static void drumkit_tick(whisper_drumkit *kit, SPFLOAT *sample)
{
    int v;
    SPFLOAT tmp;
    *sample = 0;
    for(v = 0; v < DK_NVOICES; v++) {
        drumkit_voice_tick(kit, &kit->voice[v], &tmp);
        *sample += tmp;
    }
}

void whisper_drumkit_tick(SPFLOAT *sample)
{
    drumkit_tick(&drumkit, sample);
    *sample *= 0.7f;
}

void whisper_drumkit_compute(SPFLOAT *output, unsigned int nframes)
{
    unsigned int i;
    SPFLOAT tmp;
    for (i = 0; i < nframes * 2; i += 2) {
        whisper_drumkit_tick(&tmp);
        output[i] = tmp;
        output[i + 1] = tmp;
    }
}

static void whisper_drumkit_noteon(int voice)
{
    drumkit.voice[voice].please_tick = 1;
}

static void whisper_drumkit_noteoff(int voice)
{

}

static void whisper_drumkit_set_note(int voice, int nn)
{
    int samp;
    samp = nn - DK_NOTEBIAS;
    if(samp >= 0 && samp < DK_NSAMPLES) {
        drumkit.voice[voice].next_samp = samp;
    }
}

void whisper_drumkit_bind_track(int track)
{
    whisper_tracks_set_init(track, whisper_drumkit_init);
    whisper_tracks_set_tick(track, whisper_drumkit_tick);
    whisper_tracks_set_destroy(track, whisper_drumkit_destroy);
    whisper_tracks_set_set_note(track, whisper_drumkit_set_note);
    whisper_tracks_set_noteon(track, whisper_drumkit_noteon);
    whisper_tracks_set_noteoff(track, whisper_drumkit_noteoff);
}

void whisper_drumkit_load_sample(int sample, const char *path)
{
    if(sample < 0) return;
    else if(sample >= DK_NSAMPLES) return;

    drumkit_sample_load(&drumkit, &drumkit.sample[sample], path);
}

void whisper_drumkit_default_drums()
{
    whisper_drumkit_load_sample(0, "samples/simplekit/kick.flac");
    whisper_drumkit_load_sample(1, "samples/simplekit/snare.flac");
    whisper_drumkit_load_sample(2, "samples/simplekit/hh.flac");
    /* set kit to be -1, so it knows to save */
    drumkit.kit = -1;
}

int whisper_drumkit_kit()
{
    return drumkit.kit;
}

void whisper_drumkit_kit_set(int kit)
{
    drumkit.kit = kit;
}

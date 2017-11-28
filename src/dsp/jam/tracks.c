/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include <math.h>
#include "dsp.h"

#include "note_macros.h"
#define CLIP_MAXVOICES 8 /* max voices in a clip */
#define DEFAULT_VOICES 4 /*default number of voices */
#define DB2LIN(A) pow(10, (double)(A)/20)
#define LIN2DB(A) (20*log10((double)A))
#define CLIP_MAXLEN 64

enum {
    WHISPER_PLEASE_LAUNCH,
    WHISPER_PLAYING,
    WHISPER_PLAYING_AND_QUEUED,
    WHISPER_NOT_PLAYING,
    WHISPER_CLIP_ENDED
};

struct whisper_track {
    void (*init)(int);
    void (*tick)(SPFLOAT *sample);
    void (*destroy)(void);
    void (*set_note)(int, int);
    void (*noteon)(int);
    void (*noteoff)(int);
    void *ud;
    int mode;
    int current_clip;
    int next_clip;
    whisper_clip *clip;
    
    whisper_clip *clipspace[WHISPER_NCLIPS];
    /* TODO: embed counter and ticker into layer above? */
    int counter;
    int ticker;

    int loopmode;
    SPFLOAT gain;

    int row_id;
    unsigned int timer;
    unsigned char modflag;
};


static whisper_track tracks[WHISPER_NTRACKS];

static void nothing_param(int sr) {}
static void nothing_param2(int foo, int bar) {}
static void nothing_tick(SPFLOAT *sample) { *sample = 0.f; }
static void nothing_destroy(void) {}

void whisper_tracks_init(int sr)
{
    int t, c;
    for(t = 0; t < WHISPER_NTRACKS; t++) {
        tracks[t].init = nothing_param;
        tracks[t].tick = nothing_tick;
        tracks[t].destroy = nothing_destroy;
        tracks[t].set_note = nothing_param2;
        tracks[t].noteon = nothing_param;
        tracks[t].noteoff = nothing_param;

        /* create clipspace */
        for(c = 0; c < WHISPER_NCLIPS; c++) {
            wclip_create(&tracks[t].clipspace[c], CLIP_MAXLEN, CLIP_MAXVOICES);
            wclip_set_nvoices(tracks[t].clipspace[c], DEFAULT_VOICES);
        }

        tracks[t].mode = WHISPER_NOT_PLAYING;
        tracks[t].next_clip = -2;
        tracks[t].current_clip = -1;
        tracks[t].counter = 0;
        tracks[t].ticker = 0;

        /* TODO: is this needed? */
        /* tracks[t].init(sr); */

        tracks[t].clip = tracks[t].clipspace[0];
        tracks[t].loopmode = 0;
        tracks[t].gain = 1.0;
        tracks[t].row_id = -1.0;
        tracks[t].timer = 0;
        tracks[t].modflag = 0;
    }

}

void whisper_tracks_init_instruments(int sr)
{
    int t;
    for(t = 0; t < WHISPER_NTRACKS; t++) {
        tracks[t].init(sr);
    }
}

void whisper_tracks_destroy(void)
{
    int t, c;
    for(t = 0; t < WHISPER_NTRACKS; t++) {
        tracks[t].destroy();
        for(c = 0; c < WHISPER_NCLIPS; c++) {
            wclip_destroy(&tracks[t].clipspace[c]);
        }
    }
}

static void tracks_step(whisper_track *track)
{
    int v;
    int turnoff;
    int arg;
    int nvoices;

    nvoices = wclip_get_nvoices(track->clip);

    /* check for any noteoffs at tick time */
    for(v = 0; v < nvoices; v++) {
        arg = wclip_get_arg(track->clip, v, 2);
        turnoff = (arg == track->ticker) &&
            wclip_get_gate(track->clip, v) != 0 &&
            arg > 0;
        if(turnoff) {
            track->noteoff(v);
        }
    }

    if(track->ticker == 0) {
        for(v = 0; v < nvoices; v++) {
            if(wclip_get_gate(track->clip, v) == 1) {
               track->set_note(v, wclip_get_note(track->clip, v));
               if(wclip_get_raw_note(track->clip, v) != PREV_NOTE) {
                   track->noteon(v); 
               }
            } else if(wclip_get_gate(track->clip, v) == 2) {
               track->set_note(v, 
                       wclip_get_note(track->clip, v));
            } else {
                track->noteoff(v); 
            }
        }
        if(wclip_step(track->clip)) {
            track->timer = 0;
            if(track->mode == WHISPER_PLAYING && track->loopmode == 0) {
                track->mode = WHISPER_CLIP_ENDED;
            } else if(track->mode == WHISPER_PLAYING_AND_QUEUED) {
                track->mode = WHISPER_PLEASE_LAUNCH;
            }
        }
    }
}

void whisper_tracks_tick_track(SPFLOAT *clock, SPFLOAT *sample, int tracknum)
{
    SPFLOAT tmp;
    int v;
    int loop_check;
    whisper_track *trk;
    int nvoices;

    tmp = 0.f;
    *sample = 0.f;
    
    trk = &tracks[tracknum];

    if(*clock != 0.f) {
        if(trk->ticker == 0) {
            if(trk->mode == WHISPER_CLIP_ENDED) {
                trk->mode = WHISPER_NOT_PLAYING;
                /* kill switch: turn off all voices in current clip*/
                nvoices = wclip_get_nvoices(trk->clip);
                for(v = 0; v < nvoices; v++) trk->noteoff(v);
                trk->current_clip = -1;
                trk->next_clip = -2;
            }
            if(trk->mode == WHISPER_PLEASE_LAUNCH && 
                    trk->counter == 0) {
                /* special condition: looping clips can be turned off if
                 * they are launched again */
                loop_check = trk->loopmode && 
                    (trk->current_clip == trk->next_clip);
                if(loop_check) {
                    trk->mode = WHISPER_CLIP_ENDED;
                    /* zero out next/current clips */
                    trk->current_clip = -1;
                    trk->next_clip = -2;
                } else {
                    trk->current_clip = trk->next_clip;
                    /* zero out next clip */
                    trk->next_clip = -2;
                    trk->clip = trk->clipspace[trk->current_clip];
                    trk->mode = WHISPER_PLAYING;
                    wclip_rewind(trk->clip);
		    trk->timer = 0;
                }
            }
        }

        if(trk->mode == WHISPER_PLAYING || 
            trk->mode == WHISPER_PLAYING_AND_QUEUED) {
            trk->timer++;
            tracks_step(trk);
        }

        /* TODO: put this under the bigger conditional */
        if(trk->ticker == 0) {
            trk->counter++;
            trk->counter %= 16;
        }
        trk->ticker++;
        trk->ticker %= WHISPER_NTICKS;
    }
    trk->tick(&tmp);
    *sample += tmp * trk->gain;

}

void whisper_tracks_tick(SPFLOAT *clock, SPFLOAT *sample)
{
    SPFLOAT tmp;
    int t;

    tmp = 0.f;
    *sample = 0.f;

    for(t = 0; t < WHISPER_NTRACKS; t++) {
        whisper_tracks_tick_track(clock, &tmp, t);
        *sample += tmp;
    }
}

EXPORT void whisper_schedule(int track, int clip)
{
    whisper_track *t;
    t = &tracks[track];
    
    if(t->mode == WHISPER_PLAYING || t->mode == WHISPER_PLAYING_AND_QUEUED) {
        t->mode = WHISPER_PLAYING_AND_QUEUED;
    } else {
        t->mode = WHISPER_PLEASE_LAUNCH;
    }
    t->next_clip = clip;
    /* set the currently played clip to be in the editor */
    whisper_eyejam_edit_set_track(track);
    whisper_eyejam_edit_set_clip(clip);
}

EXPORT void whisper_clip_set_note(int track, int clip, int pos, int voice, int nn)
{
    whisper_track *t;
    whisper_clip *c;

    t = &tracks[track]; 
    c = t->clipspace[clip];

    wclip_set_note(c, pos, voice, (unsigned char)nn);
}

EXPORT void whisper_clip_set_gate(int track, int clip, int pos, int voice, int gate)
{
    whisper_track *t;
    whisper_clip *c;

    t = &tracks[track]; 
    c = t->clipspace[clip];

    wclip_set_gate(c, pos, voice, (unsigned char)gate);
}

EXPORT void whisper_clip_set_arg(int track, int clip, int pos, int voice, int arg, int value)
{
    whisper_track *t;
    whisper_clip *c;

    t = &tracks[track]; 
    c = t->clipspace[clip];

    whisper_clip_modified(track, clip);
    wclip_set_arg(c, pos, voice, arg, (unsigned char)value);
}

EXPORT void whisper_clip_set_length(int track, int clip, int length)
{
    whisper_track *t;
    whisper_clip *c;

    t = &tracks[track]; 
    c = t->clipspace[clip];

    wclip_set_length(c, (unsigned int)length);
}

EXPORT void whisper_clip_set_nvoices(int track, int clip, int nvoices)
{
    whisper_track *t;
    whisper_clip *c;

    t = &tracks[track]; 
    c = t->clipspace[clip];

    wclip_set_nvoices(c, (unsigned int)nvoices);
}

void whisper_tracks_set_init(int track, void (*fun)(int))
{
    tracks[track].init = fun;    
}

void whisper_tracks_set_tick(int track, void (*fun)(SPFLOAT *))
{
    tracks[track].tick = fun;
}

void whisper_tracks_set_destroy(int track, void (*fun)(void))
{
    tracks[track].destroy = fun;
}

void whisper_tracks_set_set_note(int track, void (*fun)(int, int))
{
    tracks[track].set_note = fun;
}

void whisper_tracks_set_noteon(int track, void (*fun)(int))
{
    tracks[track].noteon = fun;
}

void whisper_tracks_set_noteoff(int track, void (*fun)(int))
{
    tracks[track].noteoff = fun;
}

EXPORT void whisper_tracks_loopmode(int track, int mode)
{
    tracks[track].loopmode = mode;
}

EXPORT void whisper_tracks_gain(int track, SPFLOAT gain)
{
    tracks[track].gain = DB2LIN(gain);
}

EXPORT int whisper_tracks_current_clip(int track)
{
    return tracks[track].current_clip;
}

EXPORT int whisper_tracks_next_clip(int track)
{
    return tracks[track].next_clip;
}

whisper_clip * whisper_clip_get(int track, int clip)
{
    return tracks[track].clipspace[clip];
}

EXPORT int whisper_tracks_get_row_id(int track)
{
    return tracks[track].row_id;
}

EXPORT void whisper_tracks_set_row_id(int track, int id)
{
    tracks[track].row_id = id;
}

EXPORT int whisper_tracks_get_clip_row_id(int track, int clip)
{
    return wclip_get_row_id(tracks[track].clipspace[clip]);
}

whisper_track * whisper_tracks_global_data()
{
    return tracks;
}

whisper_clip * wtrack_get_clip(whisper_track *track, int clip)
{
    return track->clipspace[clip];
}

whisper_track * whisper_tracks_get_track(int track)
{
    return &tracks[track];
}

SPFLOAT wtrack_get_gain(whisper_track *track)
{
    return LIN2DB(track->gain);
}

int wtrack_get_loopmode(whisper_track *track)
{
    return track->loopmode;
}

EXPORT void whisper_tracks_turnoff(int track)
{
    tracks[track].mode = WHISPER_CLIP_ENDED;
}

EXPORT void whisper_clip_clear(int track, int clip)
{
    whisper_clip_modified(track, clip);
    wclip_clear(tracks[track].clipspace[clip]);
}

EXPORT void whisper_clip_clear_note(int track, int clip, int voice, int pos)
{
    whisper_clip_modified(track, clip);
    wclip_clear_note(tracks[track].clipspace[clip], voice, pos);
}

EXPORT double whisper_tracks_get_playhead(int track)
{
    unsigned int pos;
    unsigned int cliplen;
    double val;

    cliplen = wclip_get_length(tracks[track].clip) * WHISPER_NTICKS;
    pos = tracks[track].timer;
    
    val = (double) pos / cliplen;

    return val;
}

EXPORT int whisper_tracks_am_i_alive(int track)
{
    int mode = tracks[track].mode;
    int rc;
    mode = tracks[track].mode;
    rc = 
        (mode == WHISPER_PLAYING) || 
        (mode == WHISPER_PLAYING_AND_QUEUED) ||
        (mode == WHISPER_PLEASE_LAUNCH);

    return rc;
}

EXPORT void whisper_track_modified(int track)
{
    wtrack_modified(&tracks[track]);
}

void wtrack_modified(whisper_track *track)
{
    track->modflag = 1;
}

int wtrack_is_modified(whisper_track *track)
{
    return track->modflag;
}

EXPORT int whisper_track_is_modified(int track)
{
    return wtrack_is_modified(&tracks[track]);
}

EXPORT void whisper_clip_modified(int track, int clip)
{
    whisper_clip *c;

    c = whisper_clip_get(track, clip);
    wclip_set_modflag(c);
    whisper_track_modified(track);
}

EXPORT int whisper_tracks_get_loopmode(int track)
{
    whisper_track *t;

    t = whisper_tracks_get_track(track);

    return wtrack_get_loopmode(t);
}

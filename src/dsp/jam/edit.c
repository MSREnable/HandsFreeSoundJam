/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include "dsp.h"
#include "note_macros.h"

typedef struct whisper_edit whisper_edit;

struct whisper_edit {
    int pos;
    int track;
    int clip;
    int len;
    int voice;
    int base;
};

static whisper_edit g_edit;

EXPORT void whisper_eyejam_edit_init()
{
    whisper_eyejam_edit_set_pos(0);
    whisper_eyejam_edit_set_track(0);
    whisper_eyejam_edit_set_clip(0);
    whisper_eyejam_edit_set_notelen(4);
    whisper_eyejam_edit_set_voice(0);
    whisper_eyejam_edit_set_base(60);
}

EXPORT void whisper_eyejam_edit_set_track(int track)
{
    g_edit.track = track;
}

EXPORT int whisper_eyejam_edit_get_track()
{
    return g_edit.track;
}

EXPORT void whisper_eyejam_edit_set_clip(int clip)
{
    g_edit.clip = clip;
}

EXPORT int whisper_eyejam_edit_get_clip()
{
    return g_edit.clip;
}

EXPORT void whisper_eyejam_edit_prevnote()
{
    unsigned int i;
    int len;
    int note;
    int gate;
    whisper_clip *clip;
    whisper_edit *e;

    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);
    len = wclip_get_length(clip);

    if(e->pos == 0) return;

    for(i = 0; i < len; i++) {
        e->pos--;
        if(e->pos == 0) return;
        note = wclip_get_raw_value(clip, e->voice, e->pos, 0);
        gate = wclip_get_raw_value(clip, e->voice, e->pos, 0);
        if(note != PREV_NOTE && gate != 0) {
            return;    
        }
    }

}

EXPORT void whisper_eyejam_edit_nextnote()
{
    unsigned int i;
    int len;
    int note;
    int gate;
    int ipos;
    whisper_clip *clip;
    whisper_edit *e;

    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);
    len = wclip_get_length(clip);

    if(e->pos == len) return;

    ipos = e->pos;
    for(i = ipos; i < len; i++) {
        e->pos++;
        if(e->pos == len) return;
        note = wclip_get_raw_value(clip, e->voice, e->pos, 0);
        gate = wclip_get_raw_value(clip, e->voice, e->pos, 1);
        if(note != PREV_NOTE && gate != 0) {
            return;    
        }
    }
}

EXPORT void whisper_eyejam_edit_set_pos(int pos)
{
    g_edit.pos = pos;
}

EXPORT int whisper_eyejam_edit_get_pos()
{
    return g_edit.pos;
}

EXPORT void whisper_eyejam_edit_step_right()
{
    int len;
    whisper_clip *clip;
    whisper_edit *e;

    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);
    len = wclip_get_length(clip);

    e->pos += e->len;
    if(e->pos > len) e->pos = len;
    
}

EXPORT void whisper_eyejam_edit_step_left()
{
    whisper_edit *e;

    e = &g_edit;

    e->pos -= e->len;
    if(e->pos < 0) e->pos = 0;
}

EXPORT void whisper_eyejam_edit_set_notelen(int notelen)
{
    g_edit.len = notelen;
}

EXPORT int whisper_eyejam_edit_get_notelen()
{
    return g_edit.len;
}

EXPORT void whisper_eyejam_edit_insert(int nn)
{
    unsigned int i;
    whisper_edit *e;
    e = &g_edit;
    whisper_clip_clear_note(e->track, e->clip, e->pos, e->voice);
    whisper_clip_set_note(e->track, e->clip, e->pos, e->voice, e->base + nn);
    e->pos++;
    for(i = 1; i < e->len; i++) {
        whisper_clip_clear_note(e->track, e->clip, e->pos, e->voice);
        whisper_clip_set_note(e->track, e->clip, e->pos, e->voice, PREV_NOTE);
        e->pos++;
    }

    /* set end of note to be a little bit smaller for articulation */
    whisper_clip_set_arg(e->track, e->clip, e->pos - 1, e->voice, 1, 2); 
}

EXPORT void whisper_eyejam_edit_clear()
{
    whisper_edit *e;
    e = &g_edit;
    whisper_clip_clear(e->track, e->clip);
}

EXPORT void whisper_eyejam_edit_remove()
{
    /* TODO: implement */
    unsigned int i;
    whisper_edit *e;
    unsigned int cliplen;
    whisper_clip *clip;
    int gate;
    int note;
    
    e = &g_edit;
    clip = whisper_clip_get(e->track, e->clip);
    cliplen = wclip_get_length(clip);
    note = wclip_get_raw_value(clip, e->voice, e->pos, 0);
    gate = wclip_get_raw_value(clip, e->voice, e->pos, 1);

    if(whisper_eyejam_edit_isprevnote(note) || gate == 0) {
        return;
    }
   
    whisper_clip_clear_note(e->track, e->clip, e->pos, e->voice);
    for(i = e->pos + 1; i < cliplen; i++) {
        note = wclip_get_raw_value(clip, e->voice, i, 0);
        gate = wclip_get_raw_value(clip, e->voice, i, 1);
        if(gate == 0 || !whisper_eyejam_edit_isprevnote(note)) {
            break;
        }
        whisper_clip_clear_note(e->track, e->clip, i, e->voice);
    }
}

EXPORT void whisper_eyejam_edit_set_voice(int voice)
{
    g_edit.voice = voice;
}

EXPORT int whisper_eyejam_edit_get_voice()
{
    return g_edit.voice;
}

EXPORT void whisper_eyejam_edit_set_base(int base)
{
    g_edit.base = base;
}

EXPORT int whisper_eyejam_edit_get_base()
{
    return g_edit.base;
}

EXPORT void whisper_eyejam_edit_transpose(int transpose)
{
    g_edit.base += transpose;
}

EXPORT void whisper_eyejam_edit_nextclip()
{
    g_edit.clip++;
    if(g_edit.clip >= WHISPER_NCLIPS) g_edit.clip = 0;
}

EXPORT void whisper_eyejam_edit_prevclip()
{
    g_edit.clip--;
    if(g_edit.clip < 0) g_edit.clip = WHISPER_NCLIPS - 1;
}

EXPORT void whisper_eyejam_edit_play()
{
    whisper_edit *e;

    e = &g_edit;
    whisper_schedule(e->track, e->clip);
}

EXPORT int whisper_eyejam_edit_clip_length()
{
    whisper_clip *clip;
    whisper_edit *e;
    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);

    return wclip_get_length(clip);
}

EXPORT int whisper_eyejam_edit_clip_note(int pos, int voice)
{
    whisper_clip *clip;
    whisper_edit *e;
    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);

    if(voice < 0) {
        return wclip_get_raw_value(clip, e->voice, pos, 0);
    } else {
        return wclip_get_raw_value(clip, voice, pos, 0);
    }
}

EXPORT int whisper_eyejam_edit_clip_gate(int pos, int voice)
{
    whisper_clip *clip;
    whisper_edit *e;
    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);

    if(voice < 0) {
        return wclip_get_raw_value(clip, e->voice, pos, 1);
    } else {
        return wclip_get_raw_value(clip, voice, pos, 1);
    }
}

EXPORT int whisper_eyejam_edit_isprevnote(int note)
{
    return note == PREV_NOTE;
}

EXPORT void whisper_eyejam_edit_stepsize_up()
{
    whisper_edit *e;
    e = &g_edit;
    e->len++;
}

EXPORT void whisper_eyejam_edit_stepsize_down()
{
    whisper_edit *e;
    e = &g_edit;
    e->len--;

    if(e->len <= 0) e->len =1;
}

EXPORT void whisper_eyejam_edit_stepsize_double()
{
    whisper_edit *e;
    e = &g_edit;
    e->len *= 2;
}

EXPORT void whisper_eyejam_edit_stepsize_half()
{
    whisper_edit *e;
    e = &g_edit;
    e->len /= 2;
    if(e->len <= 0) e->len = 1;
}

EXPORT int whisper_eyejam_edit_clip_nvoices()
{
    whisper_clip *clip;
    whisper_edit *e;
    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);

    return wclip_get_nvoices(clip);
}

EXPORT void whisper_eyejam_edit_voice_up()
{
    whisper_clip *clip;
    whisper_edit *e;
    e = &g_edit;

    clip = whisper_clip_get(e->track, e->clip);

    e->voice++;
    e->voice %= wclip_get_nvoices(clip);
}

EXPORT void whisper_eyejam_edit_voice_down()
{
    whisper_edit *e;
    e = &g_edit;

    e->voice--;
    if(e->voice < 0) e->voice = 0;
}

EXPORT void whisper_eyejam_edit_track_next()
{
    whisper_edit *e;
    e = &g_edit;
    e->track++;
    e->track %= WHISPER_NTRACKS;
}

EXPORT void whisper_eyejam_edit_track_prev()
{
    whisper_edit *e;
    e = &g_edit;
    e->track--;

    if(e->track < 0) e->track = WHISPER_NTRACKS - 1;
}

EXPORT double whisper_eyejam_edit_playhead()
{
    whisper_edit *e;
    e = &g_edit;

    return whisper_tracks_get_playhead(e->track);
}

EXPORT int whisper_eyejam_edit_playing()
{
    whisper_edit *e;
    e = &g_edit;

    return whisper_tracks_am_i_alive(e->track);
}

EXPORT void whisper_eyejam_edit_set_clip_length(int length)
{
    whisper_clip_set_length(
        whisper_eyejam_edit_get_track(), 
        whisper_eyejam_edit_get_clip(),
        length
    );
}

/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include "dsp.h"

#include "note_macros.h"
#define CLIP_NPARAMS 4

struct whisper_clip {
    unsigned int maxvoices;
    unsigned int nvoices;
    unsigned int length;
    unsigned int max_length;
    unsigned char *data;
    unsigned int pos;
    unsigned char *prev_note;
    size_t data_size;
    int row_id;
    unsigned char modflag;
};

void wclip_create(whisper_clip **clip, 
        unsigned int max_length, 
        unsigned int maxvoices)
{
	*clip = calloc(1, sizeof(whisper_clip));

    (*clip)->data_size = sizeof(unsigned char) * max_length * maxvoices * CLIP_NPARAMS;
	(*clip)->data = 
        calloc(1, (*clip)->data_size);
    (*clip)->prev_note = 
        calloc(1, sizeof(unsigned char) * maxvoices);
    (*clip)->length = 0;
    (*clip)->max_length = max_length;
    (*clip)->maxvoices = maxvoices;
    (*clip)->nvoices = maxvoices;
    (*clip)->pos = 0;
    (*clip)->row_id = -1;
    (*clip)->modflag = 0;
}

void wclip_set_length(whisper_clip *clip, unsigned int length)
{
    if(length <= clip->max_length) {
        /* TODO: error handling */
        clip->length = length;
    }
}

void wclip_set_nvoices(whisper_clip *clip, unsigned int nvoices)
{
    if(nvoices <= clip->maxvoices) {
        /* TODO: error handling */
        clip->nvoices = nvoices;
    }
}

void wclip_destroy(whisper_clip **clip)
{
    free((*clip)->data);
    free((*clip)->prev_note);
    free(*clip);
}

int wclip_step(whisper_clip *clip)
{
    int status;
    clip->pos++;
    status = 1;
    if(clip->pos >= clip->length) {
        clip->pos = 0;
    } else {
        status = 0;
    }

    return status;
}

int wclip_get_raw_value(whisper_clip *clip, int voice, int pos, int off)
{
    int val;
    val = (int)clip->data[pos * clip->nvoices*CLIP_NPARAMS + voice*CLIP_NPARAMS + off];
    return val;
}

int wclip_get_raw_note(whisper_clip *clip, int voice)
{
    int note;
    note = (int)clip->data[clip->pos * clip->nvoices*CLIP_NPARAMS + voice*CLIP_NPARAMS];
    return note;
}

int wclip_get_note(whisper_clip *clip, int voice)
{
    int note;
    note = wclip_get_raw_note(clip, voice);

    /* check for (and possibly reassign) previous note */
    if(note == PREV_NOTE) {
        note = (int)clip->prev_note[voice];
    } else {
        clip->prev_note[voice] = (unsigned char)note;
    }

    return note;
}

int wclip_get_arg(whisper_clip *clip, int voice, int arg)
{
    return (int)clip->data[clip->pos * clip->nvoices*CLIP_NPARAMS + voice*CLIP_NPARAMS + arg];
}

int wclip_get_gate(whisper_clip *clip, int voice)
{
    return (int)clip->data[clip->pos * clip->nvoices*CLIP_NPARAMS + voice*CLIP_NPARAMS + 1];
}

void wclip_set_gate(whisper_clip *clip, int pos, int voice, unsigned char gate)
{
    clip->data[pos * clip->nvoices*CLIP_NPARAMS + voice*CLIP_NPARAMS + 1] = gate;
}

void wclip_set_arg(whisper_clip *clip, int pos, int voice, int arg, unsigned char val)
{
    clip->data[pos * clip->nvoices*CLIP_NPARAMS + voice*CLIP_NPARAMS + 1 + arg] = val;
}

void wclip_set_note(whisper_clip *clip, int pos, int voice, unsigned char nn)
{
    clip->data[pos * clip->nvoices*CLIP_NPARAMS + voice*CLIP_NPARAMS] = nn;
    wclip_set_gate(clip, pos, voice, 1);
}

void wclip_rewind(whisper_clip *clip)
{
    clip->pos = 0;
}

unsigned char * wclip_get_data(whisper_clip *clip)
{
    return clip->data;    
}

size_t wclip_get_data_size(whisper_clip *clip)
{
    return clip->data_size;
}

int wclip_get_maxvoices(whisper_clip *clip)
{
    return clip->maxvoices;
}

int wclip_get_nvoices(whisper_clip *clip)
{
    return clip->nvoices;
}

int wclip_get_maxlength(whisper_clip *clip)
{
    return clip->max_length;
}

int wclip_get_length(whisper_clip *clip)
{
    return clip->length;
}

void wclip_set_maxvoices(whisper_clip *clip, unsigned int maxvoices)
{
    clip->maxvoices = maxvoices;
}

void wclip_set_maxlength(whisper_clip *clip, unsigned int maxlength)
{
    clip->max_length = maxlength;
}

int wclip_get_row_id(whisper_clip *clip)
{
    return clip->row_id;
}

void wclip_set_row_id(whisper_clip *clip, int row_id)
{
    clip->row_id = row_id;
}

void wclip_clear(whisper_clip *clip)
{
    unsigned int i;
    for(i = 0; i < clip->data_size; i++) {
        clip->data[i] = 0;
    }
}

void wclip_clear_note(whisper_clip *clip, int voice, int pos)
{
    unsigned int i;

    /* set note to be zero */
    wclip_set_note(clip, voice, pos, 0);

    /* zero out all arguments, which includes gate signal */
    /* NOTE: the macro CLIP_NPARAMS includes the note argument, 
     * so only do CLIP_NPARAMS-1, otherwise you'll overwrite the next 
     * voice channel. 
     */

    for(i = 0; i < CLIP_NPARAMS - 1; i++) {
        wclip_set_arg(clip, voice, pos, i, 0);
    }
}

void wclip_set_modflag(whisper_clip *clip)
{
    clip->modflag = 1;
}

int wclip_get_modflag(whisper_clip *clip)
{
    return clip->modflag;
}

void wclip_unset_modflag(whisper_clip *clip)
{
    clip->modflag = 0;
}

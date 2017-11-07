/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include "dsp.h"

#define MAX_DELAY_IN_SECONDS 5

typedef struct {
    sp_data *sp;
    sp_smoothdelay *delay[2];
    SPFLOAT gain;
    SPFLOAT tempo;
    SPFLOAT subdiv;
} delay_data;

static void set_delay_time(delay_data *dd, sp_smoothdelay *delay)
{
    SPFLOAT dtime;
    dtime = (60.f / dd->tempo) * dd->subdiv;
    delay->del = dtime;
}


static void compute(whisper_effect *e, SPFLOAT *in, SPFLOAT *L, SPFLOAT *R)
{ 
    delay_data *dd;
    int d;
    SPFLOAT tempo;
    dd = whisper_effect_data_get(e);
    tempo = whisper_eyejam_tempo_get();

    /* first, check for tempo changes */
    if(tempo != dd->tempo) {
        for(d = 0; d < 2; d++) {
                set_delay_time(dd, dd->delay[d]);
        }
    }
    dd->tempo = tempo;

    sp_smoothdelay_compute(dd->sp, dd->delay[0], in, L);
    sp_smoothdelay_compute(dd->sp, dd->delay[1], in, R);

    *L *= dd->gain;
    *R *= dd->gain;
}

static void init(whisper_effect *e, int sr)
{ 
    delay_data *dd;
    int d;

    dd = malloc(sizeof(delay_data));

    dd->sp = whisper_sp_data();
    dd->subdiv = 0.75f;

    for(d = 0; d < 2; d++) {
        sp_smoothdelay_create(&dd->delay[d]);
        sp_smoothdelay_init(dd->sp, dd->delay[d], MAX_DELAY_IN_SECONDS, 128);
        dd->delay[d]->feedback = 0.6f;
        dd->tempo = whisper_eyejam_tempo_get();
        set_delay_time(dd, dd->delay[d]);
    }

    dd->gain = 0.6f;
    whisper_effect_data_set(e, dd);
}

static void destroy(whisper_effect *e)
{
    delay_data *dd;
    int d;
    dd = whisper_effect_data_get(e);
    for(d = 0; d < 2; d++) {
        sp_smoothdelay_destroy(&dd->delay[d]);
    }
    free(dd);
}

void whisper_delay_bind(whisper_effect *effect)
{
    whisper_effect_cb_init(effect, init);
    whisper_effect_cb_compute(effect, compute);
    whisper_effect_cb_destroy(effect, destroy);
}

void whisper_delay_reset(whisper_effect *effect)
{
    /* soundpipe hack: reset delay line */
    delay_data *dd;
    dd = whisper_effect_data_get(effect);
    dd->delay[0]->counter = 0;
    dd->delay[1]->counter = 0;
}

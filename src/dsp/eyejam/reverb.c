/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include "dsp.h"

typedef struct {
    sp_data *sp;
    sp_revsc *rev;
    sp_dcblock *dcblkL;
    sp_dcblock *dcblkR;
    SPFLOAT gain;
} reverb_data;
/* Dummy functions */

static void compute(whisper_effect *e, SPFLOAT *in, SPFLOAT *L, SPFLOAT *R)
{ 
    reverb_data *rd;
    SPFLOAT s_dcblkL;
    SPFLOAT s_dcblkR;
    rd = whisper_effect_data_get(e);
    sp_revsc_compute(rd->sp, rd->rev, in, in, L, R);
    sp_dcblock_compute(rd->sp, rd->dcblkL, L, &s_dcblkL);
    sp_dcblock_compute(rd->sp, rd->dcblkR, R, &s_dcblkR);

    *L = s_dcblkL;
    *R = s_dcblkR;

    *L *= rd->gain;
    *R *= rd->gain;
}

static void init(whisper_effect *e, int sr)
{ 
    reverb_data *rd;

    rd = malloc(sizeof(reverb_data));

    rd->sp = whisper_sp_data();

    sp_revsc_create(&rd->rev);
    sp_revsc_init(rd->sp, rd->rev);
    sp_dcblock_create(&rd->dcblkL);
    sp_dcblock_init(rd->sp, rd->dcblkL);
    
    sp_dcblock_create(&rd->dcblkR);
    sp_dcblock_init(rd->sp, rd->dcblkR);

    rd->rev->feedback = 0.9f;
    rd->gain = 0.6;
    whisper_effect_data_set(e, rd);
}

static void destroy(whisper_effect *e)
{
    reverb_data *rd;
    rd = whisper_effect_data_get(e);

    sp_revsc_destroy(&rd->rev);
    sp_dcblock_destroy(&rd->dcblkL);
    sp_dcblock_destroy(&rd->dcblkR);
    free(rd);
}

void whisper_reverb_bind(whisper_effect *effect)
{
    whisper_effect_cb_init(effect, init);
    whisper_effect_cb_compute(effect, compute);
    whisper_effect_cb_destroy(effect, destroy);
}

/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include <soundpipe.h>
#include "arachnoid.h"
#include "dsp/dsp.h"

#define NCLICKERS 4

typedef struct {
    int please_tick;
    sp_osc *osc;
    sp_tenvx *env1;
    sp_tenvx *env2;
    sp_metro *m;
    SPFLOAT freq;
} clicker ;

typedef struct {
    sp_data *sp;
    sp_ftbl *ft;

    clicker c[NCLICKERS];

    int please_tick;
    int next_id;
    int next_in_line;
    
    sp_randi *ri;
    sp_vdelay *del;
    SPFLOAT prev;
} audio_data;

static audio_data G;

static void clicker_init(sp_data *sp, sp_ftbl *ft, clicker *c)
{
    sp_osc_create(&c->osc);
    sp_osc_init(sp, c->osc, ft, 0);
    c->osc->freq = 800;
    c->freq = 800;
    c->osc->amp = 0.8;

    sp_tenvx_create(&c->env1);
    sp_tenvx_init(sp, c->env1);
    c->env1->atk = 0.003;
    c->env1->hold = 0.01;
    c->env1->rel = 0.1;
    
    sp_tenvx_create(&c->env2);
    sp_tenvx_init(sp, c->env2);
    c->env2->atk = 0.001;
    c->env2->hold = 0.001;
    c->env2->rel = 0.003;

    sp_metro_create(&c->m);
    sp_metro_init(sp, c->m);
    c->m->freq = 12;
    c->please_tick = 0;
}

static void clicker_destroy(clicker *c)
{
    sp_osc_destroy(&c->osc);
    sp_tenvx_destroy(&c->env1);
    sp_tenvx_destroy(&c->env2);
    sp_metro_destroy(&c->m);
}

static SPFLOAT clicker_tick(sp_data *sp, clicker *c)
{
    SPFLOAT tmp;
    SPFLOAT env;
    SPFLOAT tick;
    SPFLOAT met;
    SPFLOAT env2;
    if(c->please_tick) {
        c->please_tick = 0;
        tick = 1;
    } else {
        tick = 0;
    }

    sp_tenvx_compute(sp, c->env1, &tick, &env);
    c->osc->freq = c->freq;
    sp_osc_compute(sp, c->osc, NULL, &tmp);

    sp_metro_compute(sp, c->m, NULL, &met);
    sp_tenvx_compute(sp, c->env2, &met, &env2);
    return tmp * env * env2; 
}

static SPFLOAT tick(audio_data *d)
{
    SPFLOAT tmp;
    SPFLOAT s_ri;
    SPFLOAT s_del;
    SPFLOAT out;
    SPFLOAT in;
    int i;
    clicker *c;
    tmp = 0;
    if(d->please_tick) {
        d->please_tick = 0;
        c = &d->c[d->next_in_line];
        c->freq = 400 + 500 * d->next_id * 0.15;
        c->m->freq = 4 + ((double)rand() / RAND_MAX) * 10;
        c->please_tick = 1;
        d->next_in_line = (d->next_in_line + 1) % NCLICKERS;
    }
    for(i = 0; i < NCLICKERS; i++) tmp += clicker_tick(d->sp, &d->c[i]);

    sp_randi_compute(d->sp, d->ri, NULL, &s_ri);
    d->del->del = s_ri;
    in = tmp + d->prev * 0.99;
    sp_vdelay_compute(d->sp, d->del, &in, &s_del);
    d->prev = s_del;
    out = s_del * 0.3 + tmp;

    /* throw to reverb in slot 0 */
    whisper_mixer_throw(0, out * 0.1);
    
    /* throw to delay in slot 1 */
    whisper_mixer_throw(1, out * 0.3);
    return out;
}

float arachnoid_tick()
{
    return tick(&G);
}

void arachnoid_audio_init()
{
    sp_data *sp;
    int i;

    G.sp = whisper_sp_data();
    sp = G.sp;

    sp_ftbl_create(sp, &G.ft, 8192);
    sp_gen_sine(sp, G.ft);

    sp_randi_create(&G.ri);
    sp_randi_init(sp, G.ri);
    G.ri->min = 0.001;
    G.ri->max = 0.1;
    G.ri->cps = 10;

    sp_vdelay_create(&G.del);
    sp_vdelay_init(sp, G.del, 0.5);
    G.prev = 0; 

    for(i = 0; i < NCLICKERS; i++) clicker_init(sp, G.ft, &G.c[i]);

    G.please_tick = 0;
    G.next_in_line = 0;
    G.next_id = 0;

}

void arachnoid_please_tick(int id)
{
    G.please_tick = 1;
    G.next_id = id;
}

void arachnoid_audio_stop()
{
    arachnoid_audio_clean();
}

void arachnoid_audio_clean()
{
    int i;
    sp_randi_destroy(&G.ri);
    sp_vdelay_destroy(&G.del);
    for(i = 0; i < NCLICKERS; i++) clicker_destroy(&G.c[i]);
    sp_ftbl_destroy(&G.ft);
}


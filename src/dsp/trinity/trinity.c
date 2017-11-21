/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <time.h>
#include "dsp.h"
#define TRINITY_NVOICES 5
#define TRINITY_NGROUPS 2
#define TRINITY_TABSIZE 16384

typedef struct trinity_osc trinity_osc;
typedef struct trinity_voice trinity_voice;
typedef struct trinity_filt trinity_filt;

static SPFLOAT compute_saw(sp_data *sp, trinity_osc *osc);
static void trinity_osc_set_tbl_sine(trinity_osc *osc, sp_ftbl *ft);
static void trinity_osc_set_tbl_saw(trinity_osc *osc, sp_ftbl *ft);
static void trinity_osc_destroy(trinity_osc *osc);
static SPFLOAT amp_env(trinity_voice *tri);
static SPFLOAT filt_env(trinity_voice *tri);
static SPFLOAT osc_freq(trinity_osc *osc);
static SPFLOAT compute_sine(sp_data *sp, trinity_osc *osc);
static void trinity_osc_sine(trinity_osc *osc);
static SPFLOAT compute_saw(sp_data *sp, trinity_osc *osc);
static void trinity_osc_init(trinity_voice *tri, trinity_osc *osc);
static void trinity_noteon(trinity_voice *tri);
static void trinity_noteoff(trinity_voice *tri);
static void gen_saw(sp_ftbl *ft);
static SPFLOAT trinity_compute(trinity_voice *tri);

static void trinity_filt_init(trinity_voice *tri, trinity_filt *filt);
static void trinity_filt_destroy(trinity_filt *filt);

typedef SPFLOAT (*osc_compute)(sp_data *sp, trinity_osc *);
typedef SPFLOAT (*filt_compute)(sp_data *, trinity_filt *, SPFLOAT);
typedef struct trinity_voice trinity_voice;

static void trinity_tick_0(SPFLOAT *sample);
static void trinity_tick_1(SPFLOAT *sample);

static void trinity_noteoff_0(int voice);
static void trinity_noteoff_1(int voice);

static void trinity_noteon_0(int voice);
static void trinity_noteon_1(int voice);

static void trinity_setnote_0(int voice, int nn);
static void trinity_setnote_1(int voice, int nn);

struct trinity_osc {
    /* ftables */
    sp_ftbl *ft_sine;
    sp_ftbl *ft_saw;
    sp_ftbl *ft_square;
    sp_ftbl *ft_triangle;

    /* oscillators */
    sp_osc *sine;
    sp_osc *saw;
    sp_osc *square;
    sp_osc *triangle;
    sp_noise *noise;
    sp_blsaw *blsaw;
    sp_blsquare *blsquare;
    sp_bltriangle *bltri;

    SPFLOAT gain;
    SPFLOAT width;
    
    SPFLOAT *note;
    unsigned int type;
    osc_compute compute;
    int is_on;
    SPFLOAT detune;
    int trans;
};

struct trinity_filt {
    sp_moogladder *moog;
    sp_butlp *butlp;
    sp_buthp *buthp;
    sp_diode *diode;
    sp_lpf18 *lpf18;
    sp_adsr *env;
    SPFLOAT env_amt;
    unsigned int type;
    filt_compute compute;
    SPFLOAT *pcutoff;
    SPFLOAT cutoff;
    SPFLOAT *res;
    SPFLOAT *dist;
};

struct trinity_voice {
    sp_data *sp;
    trinity_osc osc[3];
    trinity_filt filt;
    sp_adsr *adsr;
    sp_port *port;
    SPFLOAT *port_time;
    SPFLOAT gate;
    SPFLOAT gain;
    SPFLOAT *cutoff;
    SPFLOAT *res;
    SPFLOAT note;
    SPFLOAT inote;
    int state;
    int legato;
    int please_tick;

    /* vibrato */
    sp_osc *vib;
    SPFLOAT *pvib_depth;
    SPFLOAT *pvib_rate;
};

typedef struct {
    SPFLOAT cutoff;
    SPFLOAT res;
    SPFLOAT dist;
    SPFLOAT vib_rate;
    SPFLOAT vib_depth;
    unsigned int preset;
    trinity_voice voice[TRINITY_NVOICES];
} trinity_vgroup;

struct whisper_trinity {
    sp_data *sp;
    sp_ftbl *ft_sine;
    sp_ftbl *ft_saw;
    sp_ftbl *ft_square;
    sp_ftbl *ft_triangle;
    /* trinity_voice voice[TRINITY_NVOICES]; */
    SPFLOAT gain;
    trinity_vgroup vgroup[TRINITY_NGROUPS];
    whisper_arg1 presets[WHISPER_MAXPRESETS];
    int master;
};

/* global data */
static whisper_trinity trinity;

static whisper_compute tick_fun[] = {
    trinity_tick_0,
    trinity_tick_1
};

static whisper_arg1 noteoff_fun[] = {
    trinity_noteoff_0,
    trinity_noteoff_1
};

static whisper_arg1 noteon_fun[] = {
    trinity_noteon_0,
    trinity_noteon_1
};

static whisper_arg2 setnote_fun[] = {
    trinity_setnote_0,
    trinity_setnote_1
};

/* Oscillator */

static void trinity_osc_set_tbl_sine(trinity_osc *osc, sp_ftbl *ft)
{
    osc->ft_sine = ft;
}

static void trinity_osc_set_tbl_saw(trinity_osc *osc, sp_ftbl *ft)
{
    osc->ft_saw = ft;
}

static void trinity_osc_set_tbl_square(trinity_osc *osc, sp_ftbl *ft)
{
    osc->ft_square = ft;
}

static void trinity_osc_set_tbl_triangle(trinity_osc *osc, sp_ftbl *ft)
{
    osc->ft_triangle = ft;
}

static SPFLOAT amp_env(trinity_voice *tri)
{
    SPFLOAT s_adsr;
    sp_adsr_compute(tri->sp, tri->adsr, &tri->gate, &s_adsr);
    return s_adsr * tri->gain;
}

static SPFLOAT filt_env(trinity_voice *tri)
{
    SPFLOAT s_adsr;
    SPFLOAT g;
    g = tri->filt.env_amt;
    sp_adsr_compute(tri->sp, tri->filt.env, &tri->gate, &s_adsr);
    return *tri->filt.pcutoff * ((1.f-g) + g*s_adsr);
}

static SPFLOAT osc_freq(trinity_osc *osc)
{
    return sp_midi2cps((SPFLOAT)*osc->note + osc->detune + osc->trans);
}

static SPFLOAT compute_sine(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_osc;
    osc->sine->freq = osc_freq(osc);
    sp_osc_compute(sp, osc->sine, NULL, &s_osc);
    return s_osc * osc->gain;
}

static void trinity_osc_sine(trinity_osc *osc)
{
    osc->type = TRINITY_SINE;
    osc->compute = compute_sine;
}

static SPFLOAT compute_saw(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_osc;
    osc->saw->freq = osc_freq(osc);
    sp_osc_compute(sp, osc->saw, NULL, &s_osc);
    return s_osc * osc->gain;
}

static void trinity_osc_saw(trinity_osc *osc)
{
    osc->type = TRINITY_SAW;
    osc->compute = compute_saw;
}

static SPFLOAT compute_square(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_osc;
    osc->square->freq = osc_freq(osc);
    sp_osc_compute(sp, osc->square, NULL, &s_osc);
    return s_osc * osc->gain;
}

static void trinity_osc_square(trinity_osc *osc)
{
    osc->type = TRINITY_SQUARE;
    osc->compute = compute_square;
}

static SPFLOAT compute_triangle(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_osc;
    osc->triangle->freq = osc_freq(osc);
    sp_osc_compute(sp, osc->triangle, NULL, &s_osc);
    return s_osc * osc->gain;
}

static void trinity_osc_triangle(trinity_osc *osc)
{
    osc->type = TRINITY_TRIANGLE;
    osc->compute = compute_triangle;
}

static SPFLOAT compute_noise(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_noise;
    sp_noise_compute(sp, osc->noise, NULL, &s_noise);
    return s_noise * osc->gain;
}

static void trinity_osc_noise(trinity_osc *osc)
{
    osc->type = TRINITY_NOISE;
    osc->compute = compute_noise;
}

static SPFLOAT compute_blsaw(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_osc;
    *osc->blsaw->freq = osc_freq(osc);
    sp_blsaw_compute(sp, osc->blsaw, NULL, &s_osc);
    return s_osc * osc->gain;
}

static void trinity_osc_blsaw(trinity_osc *osc)
{
    osc->type = TRINITY_BLSAW;
    osc->compute = compute_blsaw;
}

static SPFLOAT compute_blsquare(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_osc;
    *osc->blsquare->freq = osc_freq(osc);
    *osc->blsquare->width = osc->width;
    sp_blsquare_compute(sp, osc->blsquare, NULL, &s_osc);
    return s_osc * osc->gain;
}

static void trinity_osc_blsquare(trinity_osc *osc)
{
    osc->type = TRINITY_BLSQUARE;
    osc->compute = compute_blsquare;
}

static SPFLOAT compute_bltriangle(sp_data *sp, trinity_osc *osc)
{
    SPFLOAT s_osc;
    *osc->bltri->freq = osc_freq(osc);
    sp_bltriangle_compute(sp, osc->bltri, NULL, &s_osc);
    return s_osc * osc->gain;
}

static void trinity_osc_bltriangle(trinity_osc *osc)
{
    osc->type = TRINITY_BLTRIANGLE;
    osc->compute = compute_bltriangle;
}

static void trinity_osc_init(trinity_voice *tri, trinity_osc *osc)
{
    sp_data *sp;

    sp = tri->sp;
    /* sine */
    sp_osc_create(&osc->sine);
    sp_osc_init(sp, osc->sine, osc->ft_sine, 0.f);
    osc->sine->amp = 1.0f;
   
    /* saw */
    sp_osc_create(&osc->saw);
    sp_osc_init(sp, osc->saw, osc->ft_saw, 0.f);
    osc->saw->amp = 1.0f;
    
    /* square */
    sp_osc_create(&osc->square);
    sp_osc_init(sp, osc->square, osc->ft_square, 0.f);
    osc->square->amp = 1.0f;
    
    /* triangle */
    sp_osc_create(&osc->triangle);
    sp_osc_init(sp, osc->triangle, osc->ft_triangle, 0.f);
    osc->triangle->amp = 1.0f;

    /* noise */
    sp_noise_create(&osc->noise);
    sp_noise_init(sp, osc->noise);
    osc->noise->amp = 1.f;

    /* blsaw */
    sp_blsaw_create(&osc->blsaw);
    sp_blsaw_init(sp, osc->blsaw);
    *osc->blsaw->amp = 1.f;

    /* blsquare */
    sp_blsquare_create(&osc->blsquare);
    sp_blsquare_init(sp, osc->blsquare);
    *osc->blsquare->amp = 1.f;

    /* bltriangle */
    sp_bltriangle_create(&osc->bltri);
    sp_bltriangle_init(sp, osc->bltri);
    *osc->bltri->amp = 1.f;


    /* initial values */
    osc->gain = 1.0f;
    osc->width = 0.5f;

    /* set default compute function to sine */
    trinity_osc_sine(osc);

    /* keep a pointer on the overall pitch of the note */
    osc->note = &tri->note;

    /* be turned off by default */
    osc->is_on = 0;

    /* no detune by default */
    osc->detune = 0.f;
    
    osc->trans = 0;
}

static void trinity_osc_destroy(trinity_osc *osc)
{
    sp_osc_destroy(&osc->sine);
    sp_osc_destroy(&osc->saw);
    sp_osc_destroy(&osc->square);
    sp_osc_destroy(&osc->triangle);
    sp_noise_destroy(&osc->noise);
    sp_blsaw_destroy(&osc->blsaw);
    sp_blsquare_destroy(&osc->blsquare);
    sp_bltriangle_destroy(&osc->bltri);
}


static SPFLOAT get_cutoff(trinity_filt *filt) { 
    return filt->cutoff;
}

static SPFLOAT compute_moog(sp_data *sp, trinity_filt *filt, SPFLOAT in)
{
    SPFLOAT out;
    filt->moog->freq = get_cutoff(filt);
    filt->moog->res = *filt->res;
    sp_moogladder_compute(sp, filt->moog, &in, &out);
    return out;
}

static SPFLOAT compute_butlp(sp_data *sp, trinity_filt *filt, SPFLOAT in)
{
    SPFLOAT out;
    filt->butlp->freq = get_cutoff(filt);
    sp_butlp_compute(sp, filt->butlp, &in, &out);
    return out;
}

static SPFLOAT compute_buthp(sp_data *sp, trinity_filt *filt, SPFLOAT in)
{
    SPFLOAT out;
    filt->buthp->freq = get_cutoff(filt);
    sp_buthp_compute(sp, filt->buthp, &in, &out);
    return out;
}

static SPFLOAT compute_diode(sp_data *sp, trinity_filt *filt, SPFLOAT in)
{
    SPFLOAT out;
    filt->diode->freq = get_cutoff(filt);
    filt->diode->res = *filt->res;
    sp_diode_compute(sp, filt->diode, &in, &out);
    return out;
}

static SPFLOAT compute_lpf18(sp_data *sp, trinity_filt *filt, SPFLOAT in)
{
    SPFLOAT out;
    filt->lpf18->cutoff = get_cutoff(filt);
    filt->lpf18->res = *filt->res;
    filt->lpf18->dist = *filt->dist;
    sp_lpf18_compute(sp, filt->lpf18, &in, &out);
    return out;
}

static void trinity_filt_init(trinity_voice *tri, trinity_filt *filt)
{
    sp_data *sp = tri->sp;

    sp_moogladder_create(&filt->moog);
    sp_moogladder_init(sp, filt->moog);

    sp_butlp_create(&filt->butlp);
    sp_butlp_init(sp, filt->butlp);

    sp_buthp_create(&filt->buthp);
    sp_buthp_init(sp, filt->buthp);

    sp_diode_create(&filt->diode);
    sp_diode_init(sp, filt->diode);

    sp_lpf18_create(&filt->lpf18);
    sp_lpf18_init(sp, filt->lpf18);

    filt->type = TRINITY_MOOG;
    filt->compute = compute_moog;
    filt->env_amt = 0.f;
    sp_adsr_create(&filt->env);
    sp_adsr_init(sp, filt->env);
}

static void trinity_filt_destroy(trinity_filt *filt)
{
   sp_moogladder_destroy(&filt->moog);
   sp_butlp_destroy(&filt->butlp);
   sp_buthp_destroy(&filt->buthp);
   sp_diode_destroy(&filt->diode);
   sp_lpf18_destroy(&filt->lpf18);
   sp_adsr_destroy(&filt->env);
}

static void trinity_filt_bind(trinity_vgroup *vg, trinity_filt *filt)
{
   filt->pcutoff = &vg->cutoff;
   filt->res = &vg->res;
   filt->dist = &vg->dist;
}

/* Whisper Top */

static void trinity_noteon(trinity_voice *tri)
{
    tri->gate = 1.0f;
    tri->please_tick = 1;
}

static void trinity_noteoff(trinity_voice *tri)
{
    tri->gate = 0.0f;
}

EXPORT void whisper_trinity_noteon(int voice)
{
    trinity_noteon(&trinity.vgroup[0].voice[voice]);
}

EXPORT void whisper_trinity_noteoff(int voice)
{
    trinity_noteoff(&trinity.vgroup[0].voice[voice]);
}

EXPORT void whisper_trinity_pitch(int voice, int nn)
{
    trinity.vgroup[0].voice[voice].inote = nn;
}


EXPORT void whisper_trinity_osc_sine(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];

    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_sine(&vg->voice[v].osc[osc]);
    }
}

EXPORT void whisper_trinity_osc_saw(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_saw(&vg->voice[v].osc[osc]);
    }
}

EXPORT void whisper_trinity_osc_square(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_square(&vg->voice[v].osc[osc]);
    }
}

EXPORT void whisper_trinity_osc_triangle(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_triangle(&vg->voice[v].osc[osc]);
    }
}

EXPORT void whisper_trinity_osc_blsaw(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_blsaw(&vg->voice[v].osc[osc]);
    }
}

EXPORT void whisper_trinity_osc_blsquare(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_blsquare(&vg->voice[v].osc[osc]);
    }
}

EXPORT void whisper_trinity_osc_bltriangle(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_bltriangle(&vg->voice[v].osc[osc]);
    }
}

EXPORT void whisper_trinity_osc_noise(int instr, int osc)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_noise(&vg->voice[v].osc[osc]);
    }
}

static void trinity_osc_toggle(trinity_osc *osc, int state)
{
    osc->is_on = state;
}

EXPORT void whisper_trinity_osc_toggle(int instr, int osc, int state)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_toggle(&vg->voice[v].osc[osc], state);
    }
}

static void trinity_osc_detune(trinity_osc *osc, SPFLOAT detune)
{
    osc->detune = detune;
}

EXPORT void whisper_trinity_osc_detune(int instr, int osc, SPFLOAT detune)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_detune(&vg->voice[v].osc[osc], detune);
    }
}

static void trinity_osc_transpose(trinity_osc *osc, int trans)
{
    osc->trans = trans;
}

EXPORT void whisper_trinity_osc_transpose(int instr, int osc, int trans)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_transpose(&vg->voice[v].osc[osc], trans);
    }
}

EXPORT void whisper_trinity_cutoff(int instr, SPFLOAT cutoff)
{
    trinity.vgroup[instr].cutoff = cutoff;
}

EXPORT void whisper_trinity_res(int instr, SPFLOAT res)
{
    trinity.vgroup[instr].res = res;
}

EXPORT void whisper_trinity_filt_dist(int instr, SPFLOAT dist)
{
    trinity.vgroup[instr].dist = dist;
}

static void trinity_voice_amp_atk(trinity_voice *v, SPFLOAT atk)
{
    v->adsr->atk = atk;
}

EXPORT void whisper_trinity_amp_atk(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_amp_atk(&vg->voice[v], val);
}

static void trinity_voice_amp_dec(trinity_voice *v, SPFLOAT dec)
{
    v->adsr->dec = dec;
}

EXPORT void whisper_trinity_amp_dec(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_amp_dec(&vg->voice[v], val);
}

static void trinity_voice_amp_sus(trinity_voice *v, SPFLOAT sus)
{
    v->adsr->sus= sus;
}

EXPORT void whisper_trinity_amp_sus(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_amp_sus(&vg->voice[v], val);
}

static void trinity_voice_amp_rel(trinity_voice *v, SPFLOAT rel)
{
    v->adsr->rel = rel;
}

EXPORT void whisper_trinity_amp_rel(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_amp_rel(&vg->voice[v], val);
}

static void trinity_voice_filter_atk(trinity_voice *v, SPFLOAT atk)
{
    v->filt.env->atk = atk;
}

EXPORT void whisper_trinity_filter_atk(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_filter_atk(&vg->voice[v], val);
}

static void trinity_voice_filter_dec(trinity_voice *v, SPFLOAT dec)
{
    v->filt.env->dec = dec;
}

EXPORT void whisper_trinity_filter_dec(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_filter_dec(&vg->voice[v], val);
}

static void trinity_voice_filter_sus(trinity_voice *v, SPFLOAT sus)
{
    v->filt.env->sus= sus;
}

EXPORT void whisper_trinity_filter_sus(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_filter_sus(&vg->voice[v], val);
}

static void trinity_voice_filter_rel(trinity_voice *v, SPFLOAT rel)
{
    v->filt.env->rel = rel;
}

EXPORT void whisper_trinity_filter_rel(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) 
        trinity_voice_filter_rel(&vg->voice[v], val);
}

static void trinity_voice_state(trinity_voice *v, int state)
{
    v->state = state;
}

/*TODO: make this work with multiple instruments */
EXPORT void whisper_trinity_voice_state(int instr, int voice, int state)
{
    trinity_voice_state(&trinity.vgroup[instr].voice[voice], state);
}

static void gen_saw(sp_ftbl *ft)
{
    unsigned int i;
    SPFLOAT incr;

    incr = 1.0f / (SPFLOAT)ft->size;

    for(i = 0; i < ft->size; i++) {
        ft->tbl[i] = (2.f*(i * incr) - 1.f);
    }
}

static void gen_square(sp_ftbl *ft)
{
    unsigned int i;

    for(i = 0; i < ft->size; i++) {
        if(i < ft->size / 2) {
            ft->tbl[i] = 1.f;
        } else {
            ft->tbl[i] = -1.f;
        }
    }
}

static void gen_triangle(sp_ftbl *ft)
{
    unsigned int i;
    unsigned int counter;
    SPFLOAT incr;
    int step;

    incr = 1.0f / (SPFLOAT)ft->size;
    incr *= 2;

    step = 1;

    counter = 0;
    for(i = 0; i < ft->size; i++) {
        if(i == ft->size / 2) {
            step = -1;
        }
        ft->tbl[i] = (2.f*(counter * incr) - 1.f);

        counter += step;
    }

    ft->tbl[i] = -1;
}


void whisper_trinity_init(int sr)
{
    unsigned int i;
    unsigned int v;
    unsigned int g;
    trinity_voice *voice;
    /* TODO: make global instance of soundpipe */
    /* create instance of soundpipe */
    sp_create(&trinity.sp);

    trinity.sp->sr = sr;
    /* create and initialize ftables */
    sp_ftbl_create(trinity.sp, &trinity.ft_sine, TRINITY_TABSIZE);
    sp_gen_sine(trinity.sp, trinity.ft_sine);
    sp_ftbl_create(trinity.sp, &trinity.ft_saw, TRINITY_TABSIZE);
    gen_saw(trinity.ft_saw);
    sp_ftbl_create(trinity.sp, &trinity.ft_square, TRINITY_TABSIZE);
    gen_square(trinity.ft_square);
    sp_ftbl_create(trinity.sp, &trinity.ft_triangle, TRINITY_TABSIZE);
    gen_triangle(trinity.ft_triangle);

    /* set overall gain */
    trinity.gain = 0.5f;

    /* set up presets */
    whisper_trinity_preset_setup(&trinity);
    /* set up values of instrument 0 vibrato */
    whisper_trinity_vibrato_rate(0, 0.);
    whisper_trinity_vibrato_depth(0, 0.);

    for(g = 0; g < TRINITY_NGROUPS; g++) {
        for(v = 0; v < TRINITY_NVOICES; v++) {
            voice = &trinity.vgroup[g].voice[v];
            voice->sp = trinity.sp;
            /* set ftables for oscillators to share memory */
            for(i = 0; i < 3; i++) {
                trinity_osc_set_tbl_sine(&voice->osc[i], trinity.ft_sine);
                trinity_osc_set_tbl_saw(&voice->osc[i], trinity.ft_saw);
                trinity_osc_set_tbl_square(&voice->osc[i], trinity.ft_square);
                trinity_osc_set_tbl_triangle(&voice->osc[i], trinity.ft_triangle);
                /* initialize oscillators */
                trinity_osc_init(voice, &voice->osc[i]);

                /* turn off first oscillator */
                if(i == 0) voice->osc[i].is_on = 1;

            }

            voice->gain = 1.0f;
            voice->note = 60;
            voice->inote = voice->note;
            voice->state = 0;
            
            /* amp envelope */
            sp_adsr_create(&voice->adsr);
            sp_adsr_init(voice->sp, voice->adsr);

            /* set up portamento filter for legato mode */
            voice->legato = 0;
            sp_port_create(&voice->port);
            sp_port_init(voice->sp, voice->port, 0.01f);
            voice->port_time = &voice->port->htime;
            
            voice->please_tick = 0;

            /* set up filters */
            /*
            voice->cutoff = &trinity.vgroup[0].cutoff;
            voice->res = &trinity.vgroup[0].res;
            trinity_filt_bind(&trinity.vgroup[0], &voice->filt);
            */
            trinity_filt_init(voice, &voice->filt);

            /* set up vibrato LFO */
            sp_osc_create(&voice->vib);
            sp_osc_init(voice->sp, voice->vib, trinity.ft_sine, 0.f);
            /* zero amp and freq for no effect */
            voice->vib->amp = 0.f;
            voice->vib->freq = 0.f;

            /* stuff copied from voicegroup setup loop */
            voice->cutoff = &trinity.vgroup[g].cutoff;
            voice->res = &trinity.vgroup[g].res;
            voice->pvib_depth = &trinity.vgroup[g].vib_depth;
            voice->pvib_rate = &trinity.vgroup[g].vib_rate;
            trinity_filt_bind(&trinity.vgroup[g], &voice->filt);
        }

        whisper_trinity_vibrato_rate(g, 0.);
        whisper_trinity_vibrato_depth(g, 0.);
        whisper_trinity_preset(g, trinity.vgroup[g].preset);
        trinity.vgroup[g].cutoff = 1000.f;
        trinity.vgroup[g].res = 0.f;
    }


    /* turn first voice ON by default */
    whisper_trinity_voice_state(0, 0, 1);
   
}

void whisper_trinity_destroy(void)
{
    unsigned int i, v, g;
    trinity_voice *voice;
    for(g = 0; g < TRINITY_NGROUPS; g++) {
        for(v = 0; v < TRINITY_NVOICES; v++) {
            voice = &trinity.vgroup[g].voice[v];
            for(i = 0; i < 3; i++) {
                trinity_osc_destroy(&voice->osc[i]);
            }
            sp_adsr_destroy(&voice->adsr);
            trinity_filt_destroy(&voice->filt);
            sp_port_destroy(&voice->port);
            sp_osc_destroy(&voice->vib);
        }
    }
    sp_ftbl_destroy(&trinity.ft_sine);
    sp_ftbl_destroy(&trinity.ft_saw);
    sp_ftbl_destroy(&trinity.ft_square);
    sp_ftbl_destroy(&trinity.ft_triangle);
    sp_destroy(&trinity.sp);
}

void whisper_trinity_compute(SPFLOAT *output, unsigned int nframes)
{
    unsigned int i;
    SPFLOAT tmp;
    for (i = 0; i < nframes * 2; i += 2) {
        whisper_trinity_tick(&tmp);
        output[i] = tmp;
        output[i + 1] = tmp;
    }
}

static SPFLOAT trinity_compute(trinity_voice *tri)
{
    int i;
    SPFLOAT out;
    int n;

    out = 0;
    n = 0;
    for(i = 0; i < 3; i++) {
        if(tri->osc[i].is_on) {
            out += tri->osc[i].compute(tri->sp, &tri->osc[i]);
            n++;
        }
    }

    out *= amp_env(tri) * 0.333 * n;

    /* compute filter envelope */
    tri->filt.cutoff = filt_env(tri);

    out = tri->filt.compute(tri->sp, &tri->filt, out);
    return out;
}

void whisper_trinity_tick_instr(int instr, SPFLOAT *sample)
{
    unsigned int v;
    SPFLOAT tmp;
    SPFLOAT vib;
    trinity_vgroup *vgroup;
    trinity_voice *voice;

    vgroup = &trinity.vgroup[instr];

    tmp = 0;

    for(v = 0; v < TRINITY_NVOICES; v++) {
        voice = &vgroup->voice[v];
        if(voice->state != 0){ 
            if(voice->please_tick) {
                voice->please_tick = 0;
                sp_port_reset(trinity.sp, voice->port, &voice->inote);
                
                /* 
                 * There is no sp_osc_reset yet, so reset the vibrato phase 
                 * manually.
                 */ 

                 voice->vib->lphs = 0;
            }
            voice->vib->freq = *voice->pvib_rate;
            voice->vib->amp = *voice->pvib_depth;
            sp_osc_compute(trinity.sp, voice->vib, NULL, &vib);
            if(voice->legato) {
                sp_port_compute(trinity.sp, voice->port, &voice->inote, &voice->note);
            } else {
                voice->note = voice->inote;
            }

            voice->note += vib;
            tmp += trinity_compute(voice);
        }
    }

    *sample = tmp * trinity.gain;
}

static void trinity_tick_0(SPFLOAT *sample)
{
    whisper_trinity_tick_instr(0, sample);
}

static void trinity_tick_1(SPFLOAT *sample)
{
    whisper_trinity_tick_instr(1, sample);
}

void whisper_trinity_tick(SPFLOAT *sample)
{
    whisper_trinity_tick_instr(0, sample);
}

/* HACK: implement noteoff functions for "instruments" 0 and 1.
 * This fixes a bug where turning off the bass click causes the melody
 * to "dip". These noteoff functions examine the voicegroups to see if
 * voice being turned off actually belongs to the current instrument. 
 * 
 * TODO: A better design would be to remove the concept of voicegroups and
 * de-couple trinity voices into separate instances similar to how Surgeon
 * does it. 
 */ 

static void trinity_setnote_0(int voice, int nn)
{
    trinity.vgroup[0].voice[voice].inote = nn;
}

static void trinity_setnote_1(int voice, int nn)
{
    trinity.vgroup[1].voice[voice].inote = nn;
}

static void trinity_noteon_0(int voice)
{
    trinity_vgroup *vg;
    vg = &trinity.vgroup[0];
    trinity_noteon(&vg->voice[voice]);
}

static void trinity_noteon_1(int voice)
{
    trinity_vgroup *vg;
    vg = &trinity.vgroup[1];
    trinity_noteon(&vg->voice[voice]);
}

static void trinity_noteoff_0(int voice)
{
    trinity_vgroup *vg;
    vg = &trinity.vgroup[0];
    trinity_noteoff(&vg->voice[voice]);
}

static void trinity_noteoff_1(int voice)
{
    trinity_vgroup *vg;
    vg = &trinity.vgroup[1];
    trinity_noteoff(&vg->voice[voice]);
}

static void trinity_set(int instr, int track)
{
    whisper_tracks_set_tick(track, tick_fun[instr]);
    whisper_tracks_set_set_note(track, setnote_fun[instr]);
    whisper_tracks_set_noteon(track, noteon_fun[instr]);
    whisper_tracks_set_noteoff(track, noteoff_fun[instr]);
}

void whisper_trinity_bind_track(int instr, int track)
{
    /* if this is the master instance of trinity, make it in charge of 
     * allocation and destruction of memory */
    if(track == trinity.master) {
        whisper_tracks_set_init(track, whisper_trinity_init);
        whisper_tracks_set_destroy(track, whisper_trinity_destroy);
    }

    trinity_set(instr, track);
}

static void trinity_filt_lpf18(trinity_filt *filt)
{
    filt->type = TRINITY_LPF18;
    filt->compute = compute_lpf18;
}

EXPORT void whisper_trinity_filter_lpf18(int instr)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        trinity_filt_lpf18(&vg->voice[v].filt);
    }
}

static void trinity_filt_diode(trinity_filt *filt)
{
    filt->type = TRINITY_DIODE;
    filt->compute = compute_diode;
}

EXPORT void whisper_trinity_filter_diode(int instr)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        trinity_filt_diode(&vg->voice[v].filt);
    }
}

static void trinity_filt_moog(trinity_filt *filt)
{
    filt->type = TRINITY_MOOG;
    filt->compute = compute_moog;
}

EXPORT void whisper_trinity_filter_moog(int instr)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        trinity_filt_moog(&vg->voice[v].filt);
    }
}

static void trinity_filt_butlp(trinity_filt *filt)
{
    filt->type = TRINITY_BUTLP;
    filt->compute = compute_butlp;
}

EXPORT void whisper_trinity_filter_butlp(int instr)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        trinity_filt_butlp(&vg->voice[v].filt);
    }
}

static void trinity_filt_buthp(trinity_filt *filt)
{
    filt->type = TRINITY_BUTHP;
    filt->compute = compute_buthp;
}

EXPORT void whisper_trinity_filter_buthp(int instr)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        trinity_filt_buthp(&vg->voice[v].filt);
    }
}

EXPORT void whisper_trinity_legato(int instr, int mode)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        vg->voice[v].legato = mode;
    }
}

EXPORT void whisper_trinity_port_time(int instr, SPFLOAT ptime)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        *vg->voice[v].port_time = ptime;
    }
}

whisper_arg1* whisper_trinity_preset_data(whisper_trinity *tri)
{
    return tri->presets;
}

EXPORT void whisper_trinity_preset(int instr, int preset)
{
    trinity.presets[preset](instr);
    trinity.vgroup[instr].preset = preset;
}

EXPORT void whisper_trinity_default(int instr)
{
    whisper_trinity_preset(instr, 0);
}

EXPORT int whisper_trinity_preset_number(int instr)
{
    return trinity.vgroup[instr].preset;
}

EXPORT void whisper_trinity_filter_amt(int instr, SPFLOAT val)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    for(v = 0; v < TRINITY_NVOICES; v++) {
        vg->voice[v].filt.env_amt = val;
    }
}

/* sets which track holds the master trinity track, in charge of allocation */
EXPORT void whisper_trinity_set_master(int track)
{
    trinity.master = track;
}

EXPORT void whisper_trinity_vgroup_preset(int instr, int preset)
{
    trinity.vgroup[instr].preset = preset;
}

EXPORT void whisper_trinity_vibrato_rate(int instr, SPFLOAT rate)
{
    trinity.vgroup[instr].vib_rate = rate;
}

/* Note: the depth is stored in relative MIDI values */
EXPORT void whisper_trinity_vibrato_depth(int instr, SPFLOAT depth)
{
    trinity.vgroup[instr].vib_depth = depth;
}

static void trinity_osc_gain(trinity_osc *osc, SPFLOAT gain)
{
    osc->gain = gain;
}

EXPORT void whisper_trinity_osc_gain(int instr, int osc, SPFLOAT gain)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_gain(&vg->voice[v].osc[osc], gain);
    }
}

static void trinity_osc_pulsewidth(trinity_osc *osc, SPFLOAT width)
{
    osc->width = width;
}

EXPORT void whisper_trinity_osc_pulsewidth(int instr, int osc, SPFLOAT width)
{
    unsigned int v;
    trinity_vgroup *vg;
    vg = &trinity.vgroup[instr];
    if(osc > -1 && osc < 3) {
        for(v = 0; v < TRINITY_NVOICES; v++) 
            trinity_osc_pulsewidth(&vg->voice[v].osc[osc], width);
    }
}

EXPORT void whisper_trinity_dump(int instr)
{
    FILE *fp;
    int o;
    char name[256];
    trinity_vgroup *vg;
    trinity_osc *osc;
    sp_adsr *env;


    sprintf(name, "trinity_%d.txt", (unsigned)time(NULL)); 
    
    fp = fopen(name, "w");
    vg = &trinity.vgroup[instr];
    fprintf(fp, "# BEGIN TRINITY DUMP\n\n");
    fprintf(fp, "%d filt_type\n", vg->voice[0].filt.type);
    fprintf(fp, "%g cutoff\n", vg->cutoff);
    fprintf(fp, "%g res\n", vg->res);
    fprintf(fp, "%g dist\n", vg->dist);
    env = vg->voice[0].filt.env;
    fprintf(fp, "%g filt_atk\n", env->atk);
    fprintf(fp, "%g filt_dec\n", env->dec);
    fprintf(fp, "%g filt_sus\n", env->sus);
    fprintf(fp, "%g filt_rel\n", env->rel);
    fprintf(fp, "%g filt_amt\n", vg->voice[0].filt.env_amt);
    env = vg->voice[0].adsr;
    fprintf(fp, "%g amp_atk\n", env->atk);
    fprintf(fp, "%g amp_dec\n", env->dec);
    fprintf(fp, "%g amp_sus\n", env->sus);
    fprintf(fp, "%g amp_rel\n", env->rel);

    fprintf(fp, "%g vib_rate\n", vg->vib_rate);
    fprintf(fp, "%g vib_depth\n", vg->vib_depth);
    fprintf(fp, "%d legato\n", vg->voice[0].legato);
    fprintf(fp, "%g legato_time\n", *vg->voice[0].port_time);

    for(o = 0; o < 3; o++) {
        osc = &vg->voice[0].osc[o];
        fprintf(fp, "\n%d oscillator\n", o);
        fprintf(fp, "%d osc_type\n", osc->type);
        fprintf(fp, "%g osc_gain\n", osc->gain);
        fprintf(fp, "%d osc_trans\n", osc->trans);
        fprintf(fp, "%g osc_detune\n", osc->detune);
        fprintf(fp, "%d osc_is_on\n", osc->is_on);
        fprintf(fp, "%g osc_width\n", osc->width);
    }

    fprintf(fp, "\n# END TRINITY DUMP");
    fclose(fp);
}

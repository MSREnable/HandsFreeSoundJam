#include <stdlib.h>
#include <soundpipe.h>
#include "dsp.h"
#include "moongazing.h"

#define NPADS 4
#define NCHORDS 2

/* should correspond to number of colors */

static int synth_notes[] =
{
63, 65, 67, 70, 74,
63, 67, 68, 75, 77
};

/* pitch and LFO rate */
static SPFLOAT pad_params[][2] =
{
    /* 0 2 7 11 */
    {51, 10.},
    {53, 13.},
    {58, 16.5},
    {62, 11.0},
   
    /*0 4 5 14 */ 
    {39, 10.},
    {55, 4.},
    {56, 16.5},
    {65, 11.0},
};

typedef struct {
    sp_ftbl *ft;
    sp_tenvx *env;
    sp_noise *noise;
    sp_thresh *thresh;
    sp_fosc *fm;
    SPFLOAT gate;
    SPFLOAT note;
    int *color;
} synthlet;

typedef struct {
    sp_blsaw *saw;
    sp_ftbl *ft_lfo;
    sp_osc *lfo;
    int please_change;
    SPFLOAT next_note;
    SPFLOAT next_rate;
} pad_voice;

struct mg_synth {
    sp_data *sp;
    sp_ftbl *ft;
    synthlet s[NMOONS];
    int chord;
    pad_voice pad[NPADS];
    sp_moogladder *lpf;
    sp_revsc *rev;
    sp_delay *del;
    sp_port *fade;
};

static void synthlet_set_ftbl(synthlet *s, sp_ftbl *ft)
{
    s->ft = ft;
}

static void synthlet_note(synthlet *s, int id, int chord)
{
    SPFLOAT jit;
    jit = -1.f + (((SPFLOAT)rand() / RAND_MAX) * 2.f);
    s->note = sp_midi2cps(synth_notes[id + NCOLORS * chord] + 0.1 * jit);
}

static void synthlet_init(sp_data *sp, synthlet *s)
{
    s->gate = 0.f;
    
    sp_tenvx_create(&s->env);
    sp_tenvx_init(sp, s->env);
    s->env->atk = 0.007;
    s->env->hold= 0.007;
    s->env->rel = 0.1;

    sp_thresh_create(&s->thresh);
    sp_thresh_init(sp, s->thresh);
    s->thresh->mode = 2;
    s->thresh->thresh = 0.5f;

    sp_noise_create(&s->noise);
    sp_noise_init(sp, s->noise);

    sp_fosc_create(&s->fm);
    sp_fosc_init(sp, s->fm, s->ft);
    s->fm->car = 1.f;
    s->fm->mod = 1.f;
    s->fm->indx = 1.f;
    s->fm->amp = 0.3;
}

static void synthlet_destroy(synthlet *s)
{
    sp_thresh_destroy(&s->thresh);
    sp_tenvx_destroy(&s->env);
    sp_noise_destroy(&s->noise);
    sp_fosc_destroy(&s->fm);
}

static void synthlet_tick(sp_data *sp, synthlet *s, SPFLOAT *out)
{
    SPFLOAT s_tick;
    SPFLOAT s_env;
    SPFLOAT s_fm;

    sp_thresh_compute(sp, s->thresh, &s->gate, &s_tick);
    sp_tenvx_compute(sp, s->env, &s_tick, &s_env);
    if(s_tick) {
        s->fm->freq = s->note;
        s->fm->indx = 0.5 + ((SPFLOAT)rand() / RAND_MAX) * 1.1;
    }
    sp_fosc_compute(sp, s->fm, NULL, &s_fm);

    *out = s_env * s_fm;
}

static void pad_synth_set_ftbl_lfo(pad_voice *pad, sp_ftbl *ft)
{
    pad->ft_lfo = ft;
}

static void pad_synth_init(sp_data *sp, pad_voice *pad)
{
    SPFLOAT rnd;

    sp_blsaw_create(&pad->saw);
    sp_blsaw_init(sp, pad->saw);
    *pad->saw->amp = 0.2f;

    rnd = (SPFLOAT)rand() / RAND_MAX;
    sp_osc_create(&pad->lfo);
    sp_osc_init(sp, pad->lfo, pad->ft_lfo, rnd);
    pad->lfo->amp = 1.0f;

    pad->please_change = 0;
}

static void pad_synth_destroy(pad_voice *pad)
{
    sp_blsaw_destroy(&pad->saw);
    sp_osc_destroy(&pad->lfo);
}

static void pad_synth_set_note(pad_voice *pad, SPFLOAT note)
{
    *pad->saw->freq = sp_midi2cps(note);
}

static void pad_synth_set_rate(pad_voice *pad, SPFLOAT dur)
{
    pad->lfo->freq = 1.0 / dur;
}

static void pad_synth_tick(sp_data *sp, pad_voice *pad, SPFLOAT *out)
{
    SPFLOAT s_osc;
    SPFLOAT s_lfo;

    sp_osc_compute(sp, pad->lfo, NULL, &s_lfo);

    s_lfo = 0.5*(s_lfo + 1.f);

    if(s_lfo < 0.001 && pad->please_change) {
        pad->please_change = 0;
        pad_synth_set_note(pad, pad->next_note);
        pad_synth_set_rate(pad, pad->next_rate);
    }

    sp_blsaw_compute(sp, pad->saw, NULL, &s_osc);

    *out = s_osc * s_lfo; 
}

void mg_synth_create(mg_synth **synth, int sr)
{
    mg_synth *psynth;
    sp_data *sp;
    int i;
    *synth = malloc(sizeof(mg_synth));
    psynth = *synth;
    
    sp_create(&psynth->sp);
    sp = psynth->sp;
    sp->sr = sr;
    sp_ftbl_create(sp, &psynth->ft, 8192);
    sp_gen_sine(sp, psynth->ft);

    for(i = 0; i < NMOONS; i++) {
        synthlet_set_ftbl(&psynth->s[i], psynth->ft);
        synthlet_init(sp, &psynth->s[i]);
    }

    psynth->chord = 0;
    for(i = 0; i < NPADS; i++) {
        pad_synth_set_ftbl_lfo(&psynth->pad[i], psynth->ft);

        pad_synth_init(sp, &psynth->pad[i]);
        pad_synth_set_note(&psynth->pad[i], pad_params[i][0]);
        pad_synth_set_rate(&psynth->pad[i], pad_params[i][1]);
    }

/*
    sp_revsc_create(&psynth->rev);
    sp_revsc_init(sp, psynth->rev);
    psynth->rev->feedback = 0.97f;

    sp_delay_create(&psynth->del);
    sp_delay_init(sp, psynth->del, 1.1f);
    psynth->del->feedback = 0.8f;
*/
    sp_moogladder_create(&psynth->lpf);
    sp_moogladder_init(sp, psynth->lpf);
    psynth->lpf->freq = 600.f;
    psynth->lpf->res = 0.f;

    sp_port_create(&psynth->fade);
    sp_port_init(sp, psynth->fade, 0.007);
}

void mg_synth_destroy(mg_synth **synth)
{
    mg_synth *psynth;
    int i;

    psynth = *synth;

    for(i = 0; i < NMOONS; i++) synthlet_destroy(&psynth->s[i]);
    for(i = 0; i < NPADS; i++) pad_synth_destroy(&psynth->pad[i]);
/*
    sp_delay_destroy(&psynth->del);
    sp_revsc_destroy(&psynth->rev);
*/
    sp_ftbl_destroy(&psynth->ft);
    sp_moogladder_destroy(&psynth->lpf);
    sp_port_destroy(&psynth->fade);
    sp_destroy(&psynth->sp);
    free(*synth);
}

void mg_synth_tick(mg_synth *synth, float *out)
{
    sp_data *sp;
    int i;
    SPFLOAT tmp;
    SPFLOAT dry;
    SPFLOAT pads;
    SPFLOAT s_lpf;
    SPFLOAT fade;
    SPFLOAT s_fade;

    sp = synth->sp;
    *out = 0;
    dry = 0;
    
    if(mg_time_fade() > 0.999) return;

    for(i = 0; i < NMOONS; i++) {
        synthlet_tick(sp, &synth->s[i], &tmp);
        dry += tmp;
    }

        

    pads = 0;
    for(i = 0; i < NPADS; i++) {
        pad_synth_tick(sp, &synth->pad[i], &tmp);
        pads += tmp;
    }
   
    sp_moogladder_compute(sp, synth->lpf, &pads, &s_lpf);
    dry += s_lpf * 0.25;
    *out = dry;
   
    fade = (SPFLOAT)(1 - mg_time_fade());
    /* fade out for clean exit */
    sp_port_compute(sp, synth->fade, &fade, &s_fade);
    *out *= s_fade;

    /* throw signal to reverb effect in slot 0 */
    whisper_mixer_throw(0, *out * 0.8);
    /* throw signal to delay effect in slot 1 */
    whisper_mixer_throw(1, *out * 0.8);

}

void mg_synth_trigger(mg_synth *synth, int id)
{
    synthlet *s;

    s = &synth->s[id];
    if(s->gate > 0.f) s->gate = 0.f;
    else s->gate = 1.f;

    synthlet_note(s, *s->color, synth->chord);
}

void mg_synth_bind_color(mg_synth *synth, int id, int *color)
{
    synth->s[id].color = color;
}

void mg_synth_change_chord(mg_synth *synth)
{
    int i;
    synth->chord = (synth->chord + 1) % NCHORDS;
    for(i = 0; i < NPADS; i++) {
        synth->pad[i].next_note = pad_params[synth->chord*NPADS + i][0];
        synth->pad[i].next_rate = pad_params[synth->chord*NPADS + i][1];
        synth->pad[i].please_change = 1;
    }
}

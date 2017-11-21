/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <time.h>
#include "dsp.h"
#define SURGEON_MAXVOICES 4
#define SURGEON_NSYNTH 2
#define SURGEON_NOP 4
#define SURGEON_NALGO 6

static void compute_0(SPFLOAT *);
static void init_0(int sr);
static void destroy_0(void);
typedef void (*surgeon_compute)(sp_data *, surgeon_instr *, int , SPFLOAT*);

static void surgeon_instr_setup(surgeon_instr *surg);
static void surgeon_instr_alloc(sp_data *sp, surgeon_instr *surg);

typedef struct {
    sp_adsr *adsr; /* ADSR envelope */
    sp_scale *scale; /* scaling routine to set bounds of envelope */
    SPFLOAT *min, *max; /* convenience pointers for min/max in scale */
    SPFLOAT *amount;
    SPFLOAT *atk;
    SPFLOAT *dec;
    SPFLOAT *sus;
    SPFLOAT *rel;
} envelope;

typedef struct {
    SPFLOAT scale;
    SPFLOAT freq;
    SPFLOAT index;
    SPFLOAT amp;
    SPFLOAT atk;
    SPFLOAT dec;
    SPFLOAT sus;
    SPFLOAT rel;
    SPFLOAT amount;
    int mode;
} operator_params;

typedef struct {
    sp_osc *osc;
    envelope amp_env; /* amplitude envelope */
    SPFLOAT *pfreq; /* pointer to surgeon_voice freq for relative scaling */
    SPFLOAT *freq; /* internal freq for fixed amount */
    SPFLOAT *amp; /* internal freq for fixed amount */
    int *mode;  /* 0 = fixed; 1 = relative (C:M scaling) */
    SPFLOAT *scale; /* frequency scaling when using C:M ratio */
    SPFLOAT *index; /* modulation index */
    SPFLOAT *pgate; /* pointer to gate */
} operator;

typedef struct {
    int note;
    SPFLOAT freq; /* midi2cps(note) */
    operator op[SURGEON_NOP];
    SPFLOAT gate;
} surgeon_voice;

struct surgeon_instr {
    int nvoices; /* number of active voices */
    sp_ftbl *ft; /* single sine ftable */
    surgeon_voice voice[SURGEON_MAXVOICES]; /* voice pool */
    int algo; /* oscillator configuration type */
    surgeon_compute tick[SURGEON_NALGO]; /* variable compute function */
    /* TODO: shoudln't params be SURGEON_NOP and not SURGEON_NALGO? */
    operator_params params[SURGEON_NOP]; /* operator parameters */
    int transpose;
    int preset; /* last chosen preset */
};

/* callbacks needed for track bindings */
typedef struct {
    whisper_compute compute;
    whisper_arg1 init;
    whisper_arg0 destroy;
    whisper_arg2 set_note;
    whisper_arg1 noteon;
    whisper_arg1 noteoff;
} surgeon_callback;

struct whisper_surgeon {
    sp_data *sp; /* shared soundpipe struct */
    surgeon_instr synth[SURGEON_NSYNTH]; /* synth instance */
    surgeon_callback callback[SURGEON_NSYNTH]; /*callbacks for track binding */
    whisper_arg1 preset[WHISPER_MAXPRESETS];
};

/* global data */
static whisper_surgeon surgeon;

static void nothing(sp_data *sp, surgeon_instr *instr, int voice, SPFLOAT *out)
{
    *out = 0.f;
}

static SPFLOAT env_compute(sp_data *sp, envelope *env, SPFLOAT gate)
{
    SPFLOAT e;
    SPFLOAT a = *env->amount;

    e = 0.f;
    env->adsr->atk = *env->atk;
    env->adsr->dec = *env->dec;
    env->adsr->sus = *env->sus;
    env->adsr->rel = *env->rel;
    sp_adsr_compute(sp, env->adsr, &gate, &e);

    return (1.f - a) + a*e;
}

static SPFLOAT op_compute(sp_data *sp, operator *op, SPFLOAT off)
{
    SPFLOAT tmp;
    SPFLOAT env;

    env = env_compute(sp, &op->amp_env, *op->pgate);

    switch(*op->mode) {
        case 0:
            op->osc->freq = *op->freq + off;
            op->osc->amp = *op->amp;
            break;
        case 1:
            op->osc->freq = (*op->pfreq * *op->scale) + off;
            op->osc->amp = *op->amp;
            break;
        case 2:
            op->osc->freq = *op->freq + off;
            op->osc->amp = *op->pfreq * *op->index * *op->scale;
            break;
        case 3:
            op->osc->freq = (*op->pfreq * *op->scale) + off;
            op->osc->amp = *op->pfreq * *op->index * *op->scale;
            break;
        default: /* mode 0. This shouldn't happen */
            op->osc->freq = *op->freq + off;
            op->osc->amp = *op->amp;
            /* set to mode 0 for the next loop */
            *op->mode = 0;
            break;
    }

    sp_osc_compute(sp, op->osc, NULL, &tmp);
    return tmp * env;
}

static void simple(sp_data *sp, surgeon_instr *instr, int voice, SPFLOAT *out)
{
    operator *op;
    op = &instr->voice[voice].op[0];
    *out = op_compute(sp, op, 0.f);
}

/* osc 1 modulates osc 0 */
static void classic(sp_data *sp, surgeon_instr *instr, int voice, SPFLOAT *out)
{
    operator *op;
    SPFLOAT mod;
    SPFLOAT car;
    op = instr->voice[voice].op;

    mod = op_compute(sp, &op[1], 0.f);
    car = op_compute(sp, &op[0], mod);

    *out = car;

}

/* osc 1 modulates osc 0 */
/* osc 3 modulates osc 2 */
/* osc 3 and osc 1 sum */
static void tine(sp_data *sp, surgeon_instr *instr, int voice, SPFLOAT *out)
{
    operator *op;
    SPFLOAT mod;
    SPFLOAT car;
    SPFLOAT sum;

    sum = 0;
    op = instr->voice[voice].op;

    mod = op_compute(sp, &op[1], 0.f);
    car = op_compute(sp, &op[0], mod);

    sum = car;
    
    mod = op_compute(sp, &op[3], 0.f);
    car = op_compute(sp, &op[2], mod);

    sum += car;

    *out = sum;

}

/* osc 2 modulates osc 0 */
/* osc 1 also modulates osc 0 */
/* osc 0 to DAC */
static void algo_mmc(sp_data *sp, surgeon_instr *instr, int voice, SPFLOAT *out)
{
    operator *op;
    SPFLOAT mod1;
    SPFLOAT mod2;
    SPFLOAT car;

    op = instr->voice[voice].op;

    mod1 = op_compute(sp, &op[2], 0.f);
    mod2 = op_compute(sp, &op[1], 0.f);

    car = op_compute(sp, &op[0], mod1 + mod2);


    *out = car;

}

/* all oscillators summed and sent to DAC */

static void algo_additive(sp_data *sp, surgeon_instr *instr, int voice, SPFLOAT *out)
{
    operator *op;
    SPFLOAT sum;
    int o;

    op = instr->voice[voice].op;
    sum = 0;

    for(o = 0; o < 4; o++) sum += op_compute(sp, &op[o], 0.f);

    *out = sum;

}

static void compute_0(SPFLOAT *out)
{
    int v;
    SPFLOAT tmp;
    *out = 0.f;
    tmp = 0.f;
    surgeon_instr *surg;
    surg = &surgeon.synth[0];
    for(v = 0; v < surg->nvoices; v++) {
        surgeon_compute_voice(whisper_sp_data(), surg, v, &tmp);
        *out += tmp;
    }
}

static void init_0(int sr)
{

    surgeon_instr *surg;
    surg = &surgeon.synth[0];
    surgeon_instr_alloc(whisper_sp_data(), surg);
    /* TODO: elaborate and make this a preset */
    surgeon_algo(surg, 2);

    surgeon_op_mode(surg, 0, 1);
    surgeon_op_amp(surg, 0, 0.1);
    surgeon_op_scale(surg, 0, 1);

    surgeon_op_mode(surg, 1, 3);
    surgeon_op_index(surg, 1, 1.1f);
    surgeon_op_scale(surg, 1, 1);
}

static void destroy_0(void)
{
    surgeon_instr_destroy(&surgeon.synth[0]);
}

static void set_note_0(int voice, int nn)
{
    surgeon_setnote(&surgeon.synth[0], voice, nn);
}

static void noteon_0(int voice)
{
    surgeon_noteon(&surgeon.synth[0], voice);
}

static void noteoff_0(int voice)
{
    surgeon_noteoff(&surgeon.synth[0], voice);
}

static void compute_1(SPFLOAT *out)
{
    int v;
    SPFLOAT tmp;
    *out = 0.f;
    tmp = 0.f;
    surgeon_instr *surg;
    surg = &surgeon.synth[1];
    for(v = 0; v < surg->nvoices; v++) {
        surgeon_compute_voice(whisper_sp_data(), surg, v, &tmp);
        *out += tmp;
    }
}

static void init_1(int sr)
{

    surgeon_instr *surg;
    surg = &surgeon.synth[1];
    surgeon_instr_alloc(whisper_sp_data(), surg);
    /* TODO: elaborate and make this a preset */
    surgeon_algo(surg, 3);

    surgeon_op_mode(surg, 0, 1.f);
    surgeon_op_amp(surg, 0, 0.1f);
    surgeon_op_scale(surg, 0, 1.f);
    surgeon_op_atk(surg, 0, 0.001f);
    surgeon_op_dec(surg, 0, 0.2f);
    surgeon_op_sus(surg, 0, 0.0f);
    surgeon_op_rel(surg, 0, 0.1f);

    surgeon_op_mode(surg, 1, 3.f);
    surgeon_op_index(surg, 1, 3.1f);
    surgeon_op_scale(surg, 1, 4.f);
    surgeon_op_atk(surg, 1, 0.001f);
    surgeon_op_dec(surg, 1, 0.01f);
    surgeon_op_sus(surg, 1, 0.0f);
    surgeon_op_rel(surg, 1, 0.1f);
    
    surgeon_op_mode(surg, 2, 1.f);
    surgeon_op_amp(surg, 2, 0.1f);
    surgeon_op_scale(surg, 2, 1.f);
    surgeon_op_atk(surg, 2, 0.001f);
    surgeon_op_dec(surg, 2, 0.2f);
    surgeon_op_sus(surg, 2, 0.0f);
    surgeon_op_rel(surg, 2, 0.1f);

    surgeon_op_mode(surg, 3, 3.f);
    surgeon_op_index(surg, 3, 2.1f);
    surgeon_op_scale(surg, 3, 4.005f);
    surgeon_op_atk(surg, 3, 0.001f);
    surgeon_op_dec(surg, 3, 0.01f);
    surgeon_op_sus(surg, 3, 0.0f);
    surgeon_op_rel(surg, 3, 0.1f);
}

static void destroy_1(void)
{
    surgeon_instr_destroy(&surgeon.synth[1]);
}

static void set_note_1(int voice, int nn)
{
    surgeon_setnote(&surgeon.synth[1], voice, nn);
}

static void noteon_1(int voice)
{
    surgeon_noteon(&surgeon.synth[1], voice);
}

static void noteoff_1(int voice)
{
    surgeon_noteoff(&surgeon.synth[1], voice);
}

/* initialize surgeon data struct. 
 * No malloc called here 
 */
static void surgeon_setup(whisper_surgeon *surg)
{
    unsigned int i;
    surgeon_instr *synth;
    surgeon_callback *cb;

    for(i = 0; i < SURGEON_NSYNTH; i++) {
        synth = &surg->synth[i];
        surgeon_instr_setup(synth);
    }

    cb = surg->callback;

    /* manually set up callback for each instrument */
    cb[0].compute = compute_0;
    cb[0].init = init_0;
    cb[0].destroy = destroy_0;
    cb[0].set_note = set_note_0;
    cb[0].noteon = noteon_0;
    cb[0].noteoff = noteoff_0;
    
    cb[1].compute = compute_1;
    cb[1].init = init_1;
    cb[1].destroy = destroy_1;
    cb[1].set_note = set_note_1;
    cb[1].noteon = noteon_1;
    cb[1].noteoff = noteoff_1;
}

/* zero out and initialize surgeon global data */
/* called during whisper_eyejam_init() */
void whisper_surgeon_setup()
{
    surgeon_setup(&surgeon);
    whisper_surgeon_preset_setup(&surgeon);
}

/* bind a surgeon instrument to a track */
void whisper_surgeon_bind_track(int instr, int track)
{
    whisper_tracks_set_init(track, surgeon.callback[instr].init);
    whisper_tracks_set_destroy(track, surgeon.callback[instr].destroy);
    whisper_tracks_set_tick(track, surgeon.callback[instr].compute);
    whisper_tracks_set_set_note(track, surgeon.callback[instr].set_note);
    whisper_tracks_set_noteon(track, surgeon.callback[instr].noteon);
    whisper_tracks_set_noteoff(track, surgeon.callback[instr].noteoff);
}

size_t surgeon_instr_size()
{
    return sizeof(surgeon_instr);
}

static void env_create(sp_data *sp, envelope *env)
{
    sp_adsr_create(&env->adsr);
    sp_adsr_init(sp, env->adsr);
    sp_scale_create(&env->scale);
    sp_scale_init(sp, env->scale);
    env->min = &env->scale->min;
    env->max = &env->scale->max;
}

static void env_destroy(envelope *env)
{
    sp_adsr_destroy(&env->adsr);
    sp_scale_destroy(&env->scale);
}

static void surgeon_instr_alloc(sp_data *sp, surgeon_instr *surg)
{
    unsigned int v;
    unsigned int o;
    surgeon_voice *voice;
    operator *op;

    sp_ftbl_create(sp, &surg->ft, 8192);
    sp_gen_sine(sp, surg->ft);

    surg->transpose = 0;
    for(v = 0; v < SURGEON_MAXVOICES; v++) {
        voice = &surg->voice[v];
        voice->freq = 440.f;
        voice->note = 69;
        for(o = 0; o < SURGEON_NOP; o++) {
            op = &voice->op[o];
            sp_osc_create(&op->osc);
            sp_osc_init(sp, op->osc, surg->ft, 0.f);
            env_create(sp, &op->amp_env);
        }
    }
}

/* create + init */
void surgeon_instr_init(sp_data *sp, surgeon_instr *surg)
{
    surgeon_instr_setup(surg);
    surgeon_instr_alloc(sp, surg);
}

void surgeon_instr_destroy(surgeon_instr *surg)
{
    unsigned int v;
    unsigned int o;
    surgeon_voice *voice;
    operator *op;
   
    for(v = 0; v < SURGEON_MAXVOICES; v++) {
        voice = &surg->voice[v];
        for(o = 0; o < SURGEON_NOP; o++) {
            op = &voice->op[o];
            sp_osc_destroy(&op->osc);
            env_destroy(&op->amp_env);
        }
    }


    sp_ftbl_destroy(&surg->ft);
}

void surgeon_compute_voice(sp_data *sp, surgeon_instr *ins, int v, SPFLOAT *o)
{
    ins->voice[v].freq = sp_midi2cps(ins->voice[v].note + ins->transpose);
    ins->tick[ins->algo](sp, ins, v, o);
}

/* initialize things, but do not allocate memory here */
static void surgeon_instr_setup(surgeon_instr *surg)
{
    unsigned int v;
    unsigned int o;
    surgeon_voice *voice;
    operator *op;
    operator_params *params;
    surg->nvoices = SURGEON_MAXVOICES;
    surg->algo = 0;
    surg->preset = 0;

    params = surg->params;
    for(o = 0; o < SURGEON_NOP; o++) {
        params[o].scale = 1.f;
        params[o].freq = 440.f;
        params[o].index = 1.f;
        params[o].amp = 0.5f;
        params[o].mode = 1;
        params[o].atk = 0.01f;
        params[o].dec = 0.1f;
        params[o].sus = 0.1f;
        params[o].rel = 0.1f;
        params[o].amount = 1.f;
    }
    for(v = 0; v < SURGEON_MAXVOICES; v++) {
        voice = &surg->voice[v];
        voice->freq = 440.f;
        voice->note = 69;
        voice->gate = 0.f;
        for(o = 0; o < SURGEON_NOP; o++) {
            op = &voice->op[o];
            op->pfreq = &voice->freq;
            op->amp = &params[o].amp;
            op->mode = &params[o].mode;
            op->scale = &params[o].scale;
            op->freq = &params[o].freq;
            op->index = &params[o].index;

            op->amp_env.amount = &params[o].amount;
            op->amp_env.atk = &params[o].atk;
            op->amp_env.dec = &params[o].dec;
            op->amp_env.sus = &params[o].sus;
            op->amp_env.rel = &params[o].rel;

            op->pgate = &voice->gate;
        }
    }

    /* set up algorithms to do nothing */
    for(v = 0; v < SURGEON_NALGO; v++) {
        surg->tick[v] = nothing;
    }
    surg->tick[0] = nothing;
    surg->tick[1] = simple;
    surg->tick[2] = classic;
    surg->tick[3] = tine;
    surg->tick[4] = algo_mmc;
    surg->tick[5] = algo_additive;
}

void surgeon_op_freq(surgeon_instr *surg, int osc, SPFLOAT freq)
{
    surg->params[osc].freq = freq;
}

void surgeon_op_amp(surgeon_instr *surg, int osc, SPFLOAT amp)
{
    surg->params[osc].amp = amp;
}

void surgeon_algo(surgeon_instr *surg, int algo)
{
    surg->algo = algo;
}

void surgeon_freq(surgeon_instr *surg, int voice, SPFLOAT freq)
{
    surg->voice[voice].freq = freq;
}

void surgeon_setnote(surgeon_instr *surg, int voice, int nn)
{
    surg->voice[voice].note = nn;
}

void surgeon_op_mode(surgeon_instr *surg, int osc, int mode)
{
    surg->params[osc].mode = mode;

}

void surgeon_op_index(surgeon_instr *surg, int osc, SPFLOAT index)
{
    surg->params[osc].index = index;
}

void surgeon_op_scale(surgeon_instr *surg, int osc, SPFLOAT scale)
{
    surg->params[osc].scale = scale;
}

void surgeon_op_atk(surgeon_instr *surg, int osc, SPFLOAT val)
{
    surg->params[osc].atk = val;
}

void surgeon_op_dec(surgeon_instr *surg, int osc, SPFLOAT val)
{
    surg->params[osc].dec = val;
}

void surgeon_op_sus(surgeon_instr *surg, int osc, SPFLOAT val)
{
    surg->params[osc].sus = val;
}

void surgeon_op_rel(surgeon_instr *surg, int osc, SPFLOAT val)
{
    surg->params[osc].rel = val;
}

void surgeon_op_amount(surgeon_instr *surg, int osc, SPFLOAT val)
{
    surg->params[osc].amount = val;
}

void surgeon_noteon(surgeon_instr *surg, int voice)
{
    surg->voice[voice].gate = 1.f;
}

void surgeon_noteoff(surgeon_instr *surg, int voice)
{
    surg->voice[voice].gate = 0.f;
}

void surgeon_dump(surgeon_instr *ins)
{
    FILE *fp;
    int op;
    char name[256];
    operator_params *p;

    sprintf(name, "surgeon_%d.txt", (unsigned)time(NULL)); 
    
    fp = fopen(name, "w");
    fprintf(fp, "# BEGIN SURGEON DUMP\n\n");
    fprintf(fp, "# %d nvoices\n", ins->nvoices);
    fprintf(fp, "%d algo\n", ins->algo);
    fprintf(fp, "%d transpose\n", ins->transpose);
    for(op = 0; op < SURGEON_NOP; op++) {
        p = &ins->params[op];
        fprintf(fp, "\n%d operator\n", op);
        fprintf(fp, "%g scale\n", p->scale);
        fprintf(fp, "%g freq\n", p->freq);
        fprintf(fp, "%g index\n", p->index);
        fprintf(fp, "%g amp\n", p->amp);
        fprintf(fp, "%g atk\n", p->atk);
        fprintf(fp, "%g dec\n", p->dec);
        fprintf(fp, "%g sus\n", p->sus);
        fprintf(fp, "%g rel\n", p->rel);
        fprintf(fp, "%d mode\n", p->mode);
    }
    fprintf(fp, "\n# END SURGEON DUMP");
    fclose(fp);
}

whisper_arg1* whisper_surgeon_preset_data(whisper_surgeon *surg)
{
    return surg->preset;
}

EXPORT void whisper_surgeon_preset(int instr, int preset)
{
    surgeon.preset[preset](instr);
    surgeon.synth[instr].preset = preset;
}

surgeon_instr * whisper_surgeon_get_instr(int instr)
{
    return &surgeon.synth[instr];
}

void surgeon_transpose(surgeon_instr *surg, int transpose)
{
    surg->transpose = transpose;
}

int surgeon_preset_number(surgeon_instr *ins)
{
    return ins->preset;
}

EXPORT int whisper_surgeon_preset_number(int instr)
{
    return surgeon_preset_number(&surgeon.synth[instr]);
}

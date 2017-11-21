#include <stdio.h>
#include <runt.h>
#include "dsp.h"

#define P0(X) printf("    "X)
#define P1(X,A) printf("    "X,A)
#define P2(X,A0,A1) printf("    "X,A0,A1)

#define PARAM(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P1(S"(instr, %ff);\n", val);

#define PARAMI(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P1(S"(instr, %g);\n", val);

#define OSC_PARAM(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P2(S"(instr, %d, %ff);\n", osc, val);

#define OSC_PARAMI(S) \
    runt_int rc;\
    runt_float val;\
    rc = pop_float(vm, &val);\
    RUNT_ERROR_CHECK(rc);\
    P2(S"(instr, %d, %g);\n", osc, val);

static int osc;

static runt_int pop_float(runt_vm *vm, runt_float *f)
{
    runt_int rc;
    runt_stacklet *s;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    *f = s->f;

    return RUNT_OK;
}

static runt_int rproc_filt_type(runt_vm *vm, runt_ptr p)
{
    runt_float f;
    runt_int type;
    runt_int rc;

    rc = pop_float(vm, &f);
    RUNT_ERROR_CHECK(rc);
   
    type = f;
    switch(type)  {
        case TRINITY_MOOG:
            P0("whisper_trinity_filter_moog(instr);\n");
            break;
        case TRINITY_BUTLP:
            P0("whisper_trinity_filter_butlp(instr);\n");
            break;
        case TRINITY_DIODE:
            P0("whisper_trinity_filter_diode(instr);\n");
            break;
        case TRINITY_BUTHP:
            P0("whisper_trinity_filter_buthp(instr);\n");
            break;
        case TRINITY_LPF18:
            P0("whisper_trinity_filter_lpf18(instr);\n");
            break;
        default:
            break;
    }

    
    return RUNT_OK;
}

static runt_int rproc_cutoff(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_cutoff")
    return RUNT_OK;
}

static runt_int rproc_res(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_res")
    return RUNT_OK;
}

static runt_int rproc_dist(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_filt_dist")
    return RUNT_OK;
}

static runt_int rproc_filt_atk(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_filter_atk");
    return RUNT_OK;
}

static runt_int rproc_filt_dec(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_filter_dec");
    return RUNT_OK;
}

static runt_int rproc_filt_sus(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_filter_sus");
    return RUNT_OK;
}

static runt_int rproc_filt_rel(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_filter_rel");
    return RUNT_OK;
}

static runt_int rproc_filt_amt(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_filter_amt");
    return RUNT_OK;
}


static runt_int rproc_amp_atk(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_amp_atk");
    return RUNT_OK;
}

static runt_int rproc_amp_dec(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_amp_dec");
    return RUNT_OK;
}

static runt_int rproc_amp_sus(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_amp_sus");
    return RUNT_OK;
}

static runt_int rproc_amp_rel(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_amp_rel");
    return RUNT_OK;
}

static runt_int rproc_vib_rate(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_vibrato_rate");
    return RUNT_OK;
}

static runt_int rproc_vib_depth(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_vibrato_depth");
    return RUNT_OK;
}

static runt_int rproc_legato(runt_vm *vm, runt_ptr p)
{
    PARAMI("whisper_trinity_legato");
    return RUNT_OK;
}

static runt_int rproc_legato_time(runt_vm *vm, runt_ptr p)
{
    PARAM("whisper_trinity_port_time");
    return RUNT_OK;
}

static runt_int rproc_oscillator(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_float val;
    
    rc = pop_float(vm, &val);
    RUNT_ERROR_CHECK(rc);

    osc = val;
    return RUNT_OK;
}

static runt_int rproc_osc_type(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_float val;
    runt_int type;
    
    rc = pop_float(vm, &val);
    RUNT_ERROR_CHECK(rc);

    type = val;

    switch(type)
    {
        case TRINITY_SINE:
            P1("whisper_trinity_osc_sine(instr, %d);\n", osc);
            break;
        case TRINITY_SAW:
            P1("whisper_trinity_osc_saw(instr, %d);\n", osc);
            break;
        case TRINITY_TRIANGLE:
            P1("whisper_trinity_osc_triangle(instr, %d);\n", osc);
            break;
        case TRINITY_SQUARE:
            P1("whisper_trinity_osc_square(instr, %d);\n", osc);
            break;
        case TRINITY_NOISE:
            P1("whisper_trinity_osc_noise(instr, %d);\n", osc);
            break;
        case TRINITY_BLSAW:
            P1("whisper_trinity_osc_blsaw(instr, %d);\n", osc);
            break;
        case TRINITY_BLSQUARE:
            P1("whisper_trinity_osc_blsquare(instr, %d);\n", osc);
            break;
        case TRINITY_BLTRIANGLE:
            P1("whisper_trinity_osc_bltriangle(instr, %d);\n", osc);
            break;
    }

    return RUNT_OK;
}

static runt_int rproc_osc_gain(runt_vm *vm, runt_ptr p)
{
    OSC_PARAM("whisper_trinity_osc_gain");
    return RUNT_OK;
}

static runt_int rproc_osc_trans(runt_vm *vm, runt_ptr p)
{
    OSC_PARAM("whisper_trinity_osc_transpose");
    return RUNT_OK;
}

static runt_int rproc_osc_detune(runt_vm *vm, runt_ptr p)
{
    OSC_PARAM("whisper_trinity_osc_detune");
    return RUNT_OK;
}

static runt_int rproc_osc_is_on(runt_vm *vm, runt_ptr p)
{
    OSC_PARAMI("whisper_trinity_osc_toggle");
    return RUNT_OK;
}

static runt_int rproc_osc_width(runt_vm *vm, runt_ptr p)
{
    OSC_PARAM("whisper_trinity_osc_pulsewidth");
    return RUNT_OK;
}

static runt_int loader(runt_vm *vm)
{
    runt_keyword_define(vm, "filt_type", 9, rproc_filt_type, NULL);
    runt_keyword_define(vm, "cutoff", 6, rproc_cutoff, NULL);
    runt_keyword_define(vm, "res", 3, rproc_res, NULL);
    runt_keyword_define(vm, "dist", 4, rproc_dist, NULL);
    runt_keyword_define(vm, "filt_atk", 8, rproc_filt_atk, NULL);
    runt_keyword_define(vm, "filt_dec", 8, rproc_filt_dec, NULL);
    runt_keyword_define(vm, "filt_sus", 8, rproc_filt_sus, NULL);
    runt_keyword_define(vm, "filt_rel", 8, rproc_filt_rel, NULL);
    runt_keyword_define(vm, "filt_amt", 8, rproc_filt_amt, NULL);
    runt_keyword_define(vm, "amp_atk", 7, rproc_amp_atk, NULL);
    runt_keyword_define(vm, "amp_dec", 7, rproc_amp_dec, NULL);
    runt_keyword_define(vm, "amp_sus", 7, rproc_amp_sus, NULL);
    runt_keyword_define(vm, "amp_rel", 7, rproc_amp_rel, NULL);
    runt_keyword_define(vm, "vib_rate", 8, rproc_vib_rate, NULL);
    runt_keyword_define(vm, "vib_depth", 9, rproc_vib_depth, NULL);
    runt_keyword_define(vm, "legato", 6, rproc_legato, NULL);
    runt_keyword_define(vm, "legato_time", 11, rproc_legato_time, NULL);
    runt_keyword_define(vm, "oscillator", 10, rproc_oscillator, NULL);
    runt_keyword_define(vm, "osc_type", 8, rproc_osc_type, NULL);
    runt_keyword_define(vm, "osc_gain", 8, rproc_osc_gain, NULL);
    runt_keyword_define(vm, "osc_trans", 9, rproc_osc_trans, NULL);
    runt_keyword_define(vm, "osc_detune", 10, rproc_osc_detune, NULL);
    runt_keyword_define(vm, "osc_is_on", 9, rproc_osc_is_on, NULL);
    runt_keyword_define(vm, "osc_width", 9, rproc_osc_width, NULL);
    return runt_is_alive(vm);
}

int main(int argc, char *argv[])
{
    int rc;
    printf("static void trinity_PRESET(int instr)\n{\n");
    rc = irunt_begin(argc, argv, loader);
    printf("}\n");
    return rc;
}

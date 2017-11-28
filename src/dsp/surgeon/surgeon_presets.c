/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <time.h>
#include "dsp.h"

#define SURGEON_ACTIVE_PRESETS 8

typedef void (*surgeon_preset_proc)(surgeon_instr*);

static surgeon_preset_proc presets[WHISPER_MAXPRESETS];

static const char *surgeon_preset_names[] = {
    "Default Sound",
    "Default Pad",
    "Default Pluck",
    "Toy Piano",
    "Chorus Piano",
    "Organ",
    "Clangy Bell",
    "Deep Bass"
};

static void surgeon_toypiano(surgeon_instr *surg)
{
    surgeon_algo(surg, 4);
    surgeon_transpose(surg, 0);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 0.100000f);
    surgeon_op_atk(surg, 0, 0.004000f);
    surgeon_op_dec(surg, 0, 0.100000f);
    surgeon_op_sus(surg, 0, 0.000000f);
    surgeon_op_rel(surg, 0, 0.100000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 7.000000f);
    surgeon_op_freq(surg, 1, 1.000000f);
    surgeon_op_index(surg, 1, 1.000000f);
    surgeon_op_amp(surg, 1, 100.000000f);
    surgeon_op_atk(surg, 1, 0.001000f);
    surgeon_op_dec(surg, 1, 0.200000f);
    surgeon_op_sus(surg, 1, 1.000000f);
    surgeon_op_rel(surg, 1, 0.500000f);
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_scale(surg, 2, 2.000000f);
    surgeon_op_freq(surg, 2, 6.000000f);
    surgeon_op_index(surg, 2, 7.000000f);
    surgeon_op_amp(surg, 2, 10.000000f);
    surgeon_op_atk(surg, 2, 0.001000f);
    surgeon_op_dec(surg, 2, 0.010000f);
    surgeon_op_sus(surg, 2, 0.100000f);
    surgeon_op_rel(surg, 2, 0.100000f);
    surgeon_op_mode(surg, 2, 3);
    surgeon_op_scale(surg, 3, 4.000000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 1.000000f);
    surgeon_op_amp(surg, 3, 0.100000f);
    surgeon_op_atk(surg, 3, 0.010000f);
    surgeon_op_dec(surg, 3, 0.100000f);
    surgeon_op_sus(surg, 3, 0.100000f);
    surgeon_op_rel(surg, 3, 0.100000f);
    surgeon_op_mode(surg, 3, 3);
}

static void surgeon_default(surgeon_instr *surg)
{
    surgeon_algo(surg, 2);
    surgeon_transpose(surg, 0);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 0.100000f);
    surgeon_op_atk(surg, 0, 0.010000f);
    surgeon_op_dec(surg, 0, 0.100000f);
    surgeon_op_sus(surg, 0, 0.100000f);
    surgeon_op_rel(surg, 0, 0.100000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 1.000000f);
    surgeon_op_freq(surg, 1, 1.000000f);
    surgeon_op_index(surg, 1, 1.000000f);
    surgeon_op_amp(surg, 1, 100.000000f);
    surgeon_op_atk(surg, 1, 0.010000f);
    surgeon_op_dec(surg, 1, 0.100000f);
    surgeon_op_sus(surg, 1, 0.100000f);
    surgeon_op_rel(surg, 1, 0.100000f);
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_scale(surg, 2, 1.000000f);
    surgeon_op_freq(surg, 2, 440.000000f);
    surgeon_op_index(surg, 2, 1.000000f);
    surgeon_op_amp(surg, 2, 0.100000f);
    surgeon_op_atk(surg, 2, 0.010000f);
    surgeon_op_dec(surg, 2, 0.100000f);
    surgeon_op_sus(surg, 2, 0.100000f);
    surgeon_op_rel(surg, 2, 0.100000f);
    surgeon_op_mode(surg, 2, 1);
    surgeon_op_scale(surg, 3, 1.000000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 1.000000f);
    surgeon_op_amp(surg, 3, 0.500000f);
    surgeon_op_atk(surg, 3, 0.010000f);
    surgeon_op_dec(surg, 3, 0.100000f);
    surgeon_op_sus(surg, 3, 0.100000f);
    surgeon_op_rel(surg, 3, 0.100000f);
    surgeon_op_mode(surg, 3, 1);
}

static void surgeon_choruspiano(surgeon_instr *surg)
{
    surgeon_algo(surg, 3);
    surgeon_transpose(surg, 0);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 0.0700000f);
    surgeon_op_atk(surg, 0, 0.001000f);
    surgeon_op_dec(surg, 0, 0.400000f);
    surgeon_op_sus(surg, 0, 0.000000f);
    surgeon_op_rel(surg, 0, 0.100000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 6.990000f);
    surgeon_op_freq(surg, 1, 1.000000f);
    surgeon_op_index(surg, 1, 2.000000f);
    surgeon_op_amp(surg, 1, 100.000000f);
    surgeon_op_atk(surg, 1, 0.001000f);
    surgeon_op_dec(surg, 1, 0.100000f);
    surgeon_op_sus(surg, 1, 0.100000f);
    surgeon_op_rel(surg, 1, 0.010000f);
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_scale(surg, 2, 1.000000f);
    surgeon_op_freq(surg, 2, 6.000000f);
    surgeon_op_index(surg, 2, 1.000000f);
    surgeon_op_amp(surg, 2, 0.0700000f);
    surgeon_op_atk(surg, 2, 0.001000f);
    surgeon_op_dec(surg, 2, 0.400000f);
    surgeon_op_sus(surg, 2, 0.000000f);
    surgeon_op_rel(surg, 2, 0.020000f);
    surgeon_op_mode(surg, 2, 1);
    surgeon_op_scale(surg, 3, 7.000000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 2.000000f);
    surgeon_op_amp(surg, 3, 0.0700000f);
    surgeon_op_atk(surg, 3, 0.001000f);
    surgeon_op_dec(surg, 3, 0.100000f);
    surgeon_op_sus(surg, 3, 0.100000f);
    surgeon_op_rel(surg, 3, 0.010000f);
    surgeon_op_mode(surg, 3, 3);
}

static void surgeon_organ(surgeon_instr *surg)
{
    surgeon_algo(surg, 5);
    surgeon_transpose(surg, 0);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 0.0700000f);
    surgeon_op_atk(surg, 0, 0.010000f);
    surgeon_op_dec(surg, 0, 0.010000f);
    surgeon_op_sus(surg, 0, 0.890000f);
    surgeon_op_rel(surg, 0, 0.001000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 2.000000f);
    surgeon_op_freq(surg, 1, 1.000000f);
    surgeon_op_index(surg, 1, 1.000000f);
    surgeon_op_amp(surg, 1, 0.0700000f);
    surgeon_op_atk(surg, 1, 0.010000f);
    surgeon_op_dec(surg, 1, 0.010000f);
    surgeon_op_sus(surg, 1, 0.800000f);
    surgeon_op_rel(surg, 1, 0.001000f);
    surgeon_op_mode(surg, 1, 1);
    surgeon_op_scale(surg, 2, 3.000000f);
    surgeon_op_freq(surg, 2, 6.000000f);
    surgeon_op_index(surg, 2, 1.000000f);
    surgeon_op_amp(surg, 2, 0.030000f);
    surgeon_op_atk(surg, 2, 0.010000f);
    surgeon_op_dec(surg, 2, 0.010000f);
    surgeon_op_sus(surg, 2, 0.800000f);
    surgeon_op_rel(surg, 2, 0.100000f);
    surgeon_op_mode(surg, 2, 1);
    surgeon_op_scale(surg, 3, 0.500000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 1.000000f);
    surgeon_op_amp(surg, 3, 0.200000f);
    surgeon_op_atk(surg, 3, 0.010000f);
    surgeon_op_dec(surg, 3, 0.010000f);
    surgeon_op_sus(surg, 3, 0.800000f);
    surgeon_op_rel(surg, 3, 0.100000f);
    surgeon_op_mode(surg, 3, 1);
}

static void surgeon_clangybell(surgeon_instr *surg)
{
    surgeon_algo(surg, 3);
    surgeon_transpose(surg, -24);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 0.400000f);
    surgeon_op_atk(surg, 0, 0.001000f);
    surgeon_op_dec(surg, 0, 0.500000f);
    surgeon_op_sus(surg, 0, 0.000000f);
    surgeon_op_rel(surg, 0, 0.001000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 3.168200f);
    surgeon_op_freq(surg, 1, 1.000000f);
    surgeon_op_index(surg, 1, 8.000000f);
    surgeon_op_amp(surg, 1, 100.000000f);
    surgeon_op_atk(surg, 1, 0.001000f);
    surgeon_op_dec(surg, 1, 0.900000f);
    surgeon_op_sus(surg, 1, 0.000000f);
    surgeon_op_rel(surg, 1, 0.010000f);
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_scale(surg, 2, 2.000000f);
    surgeon_op_freq(surg, 2, 440.000000f);
    surgeon_op_index(surg, 2, 1.000000f);
    surgeon_op_amp(surg, 2, 0.900000f);
    surgeon_op_atk(surg, 2, 0.001000f);
    surgeon_op_dec(surg, 2, 0.010000f);
    surgeon_op_sus(surg, 2, 0.000000f);
    surgeon_op_rel(surg, 2, 0.001000f);
    surgeon_op_mode(surg, 2, 1);
    surgeon_op_scale(surg, 3, 1.000000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 3.000000f);
    surgeon_op_amp(surg, 3, 0.500000f);
    surgeon_op_atk(surg, 3, 0.010000f);
    surgeon_op_dec(surg, 3, 0.100000f);
    surgeon_op_sus(surg, 3, 0.100000f);
    surgeon_op_rel(surg, 3, 0.100000f);
    surgeon_op_mode(surg, 3, 3);
}


static void surgeon_deepbass(surgeon_instr *surg)
{
    surgeon_algo(surg, 3);
    surgeon_transpose(surg, -36);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 1.000000f);
    surgeon_op_atk(surg, 0, 0.001000f);
    surgeon_op_dec(surg, 0, 0.010000f);
    surgeon_op_sus(surg, 0, 0.500000f);
    surgeon_op_rel(surg, 0, 0.001000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 1.005000f);
    surgeon_op_freq(surg, 1, 1.000000f);
    surgeon_op_index(surg, 1, 15.000000f);
    surgeon_op_amp(surg, 1, 100.000000f);
    surgeon_op_atk(surg, 1, 0.001000f);
    surgeon_op_dec(surg, 1, 0.001000f);
    surgeon_op_sus(surg, 1, 0.400000f);
    surgeon_op_rel(surg, 1, 0.002000f);
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_scale(surg, 2, 4.000000f);
    surgeon_op_freq(surg, 2, 0.000000f);
    surgeon_op_index(surg, 2, 1.000000f);
    surgeon_op_amp(surg, 2, 0.100000f);
    surgeon_op_atk(surg, 2, 0.001000f);
    surgeon_op_dec(surg, 2, 0.010000f);
    surgeon_op_sus(surg, 2, 0.300000f);
    surgeon_op_rel(surg, 2, 0.001000f);
    surgeon_op_mode(surg, 2, 1);
    surgeon_op_scale(surg, 3, 4.000000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 3.000000f);
    surgeon_op_amp(surg, 3, 0.500000f);
    surgeon_op_atk(surg, 3, 0.004000f);
    surgeon_op_dec(surg, 3, 0.100000f);
    surgeon_op_sus(surg, 3, 0.500000f);
    surgeon_op_rel(surg, 3, 0.002000f);
    surgeon_op_mode(surg, 3, 3);
}

static void surgeon_defaultpluck(surgeon_instr *surg)
{
    surgeon_algo(surg, 3);
    surgeon_transpose(surg, 0);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 0.100000f);
    surgeon_op_atk(surg, 0, 0.001000f);
    surgeon_op_dec(surg, 0, 0.200000f);
    surgeon_op_sus(surg, 0, 0.000000f);
    surgeon_op_rel(surg, 0, 0.100000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 4.000000f);
    surgeon_op_freq(surg, 1, 440.000000f);
    surgeon_op_index(surg, 1, 3.100000f);
    surgeon_op_amp(surg, 1, 0.500000f);
    surgeon_op_atk(surg, 1, 0.001000f);
    surgeon_op_dec(surg, 1, 0.010000f);
    surgeon_op_sus(surg, 1, 0.000000f);
    surgeon_op_rel(surg, 1, 0.100000f);
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_scale(surg, 2, 1.000000f);
    surgeon_op_freq(surg, 2, 440.000000f);
    surgeon_op_index(surg, 2, 1.000000f);
    surgeon_op_amp(surg, 2, 0.100000f);
    surgeon_op_atk(surg, 2, 0.001000f);
    surgeon_op_dec(surg, 2, 0.200000f);
    surgeon_op_sus(surg, 2, 0.000000f);
    surgeon_op_rel(surg, 2, 0.100000f);
    surgeon_op_mode(surg, 2, 1);
    surgeon_op_scale(surg, 3, 4.005000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 2.100000f);
    surgeon_op_amp(surg, 3, 0.500000f);
    surgeon_op_atk(surg, 3, 0.001000f);
    surgeon_op_dec(surg, 3, 0.010000f);
    surgeon_op_sus(surg, 3, 0.000000f);
    surgeon_op_rel(surg, 3, 0.100000f);
    surgeon_op_mode(surg, 3, 3);
}

static void surgeon_defaultpad(surgeon_instr *surg)
{
    surgeon_algo(surg, 2);
    surgeon_transpose(surg, 0);
    surgeon_op_scale(surg, 0, 1.000000f);
    surgeon_op_freq(surg, 0, 440.000000f);
    surgeon_op_index(surg, 0, 1.000000f);
    surgeon_op_amp(surg, 0, 0.100000f);
    surgeon_op_atk(surg, 0, 0.010000f);
    surgeon_op_dec(surg, 0, 0.100000f);
    surgeon_op_sus(surg, 0, 0.100000f);
    surgeon_op_rel(surg, 0, 0.100000f);
    surgeon_op_mode(surg, 0, 1);
    surgeon_op_scale(surg, 1, 1.000000f);
    surgeon_op_freq(surg, 1, 440.000000f);
    surgeon_op_index(surg, 1, 1.100000f);
    surgeon_op_amp(surg, 1, 0.500000f);
    surgeon_op_atk(surg, 1, 0.010000f);
    surgeon_op_dec(surg, 1, 0.100000f);
    surgeon_op_sus(surg, 1, 0.100000f);
    surgeon_op_rel(surg, 1, 0.100000f);
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_scale(surg, 2, 1.000000f);
    surgeon_op_freq(surg, 2, 440.000000f);
    surgeon_op_index(surg, 2, 1.000000f);
    surgeon_op_amp(surg, 2, 0.500000f);
    surgeon_op_atk(surg, 2, 0.010000f);
    surgeon_op_dec(surg, 2, 0.100000f);
    surgeon_op_sus(surg, 2, 0.100000f);
    surgeon_op_rel(surg, 2, 0.100000f);
    surgeon_op_mode(surg, 2, 1);
    surgeon_op_scale(surg, 3, 1.000000f);
    surgeon_op_freq(surg, 3, 440.000000f);
    surgeon_op_index(surg, 3, 1.000000f);
    surgeon_op_amp(surg, 3, 0.500000f);
    surgeon_op_atk(surg, 3, 0.010000f);
    surgeon_op_dec(surg, 3, 0.100000f);
    surgeon_op_sus(surg, 3, 0.100000f);
    surgeon_op_rel(surg, 3, 0.100000f);
    surgeon_op_mode(surg, 3, 1);
}

void surgeon_preset_list_init()
{
    int i;
    for(i = 0; i < WHISPER_MAXPRESETS; i++) {
        presets[i] = surgeon_default;
    }
    presets[1] = surgeon_defaultpad;
    presets[2] = surgeon_defaultpluck;
    presets[3] = surgeon_toypiano;
    presets[4] = surgeon_choruspiano;
    presets[5] = surgeon_organ;
    presets[6] = surgeon_clangybell;
    presets[7] = surgeon_deepbass;
}

void surgeon_preset(surgeon_instr *ins, int preset)
{
    if(preset >= 0 && preset < WHISPER_MAXPRESETS) presets[preset](ins);
}

EXPORT void whisper_surgeon_preset_next(int instr)
{
    unsigned int current;

    current = whisper_surgeon_preset_number(instr);
    whisper_surgeon_preset(instr, (current + 1) % SURGEON_ACTIVE_PRESETS);
}

EXPORT void whisper_surgeon_preset_prev(int instr)
{
    unsigned int current;
    current = whisper_surgeon_preset_number(instr);
    if(current == 0) current = SURGEON_ACTIVE_PRESETS - 1;
    else current--;
    whisper_surgeon_preset(instr, current);
}

EXPORT const char * whisper_surgeon_preset_name(int preset)
{
    /* bounds checking */
    if(preset < 0) preset = 0;
    preset %= SURGEON_ACTIVE_PRESETS;
    return surgeon_preset_names[preset];
}

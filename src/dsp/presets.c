#include "dsp.h"

static void trinity_default(int instr)
{
    /* toggle all oscillators */
    whisper_trinity_osc_toggle(instr, 0, 1);
    whisper_trinity_osc_toggle(instr, 1, 1);
    whisper_trinity_osc_toggle(instr, 2, 1);

    /* detune 1 + 3 oscillators */
    whisper_trinity_osc_detune(instr, 0, -0.05);
    whisper_trinity_osc_detune(instr, 2, 0.07);

    /* all saw */
    whisper_trinity_osc_saw(instr, 0);
    whisper_trinity_osc_saw(instr, 1);

    /* filter */
    whisper_trinity_cutoff(instr, 3000.f);
    whisper_trinity_res(instr, 0.);

    /* ADSR envelop */

    whisper_trinity_amp_atk(instr, 0.005f);
    whisper_trinity_amp_dec(instr, 0.1f);
    whisper_trinity_amp_sus(instr, 0.5f);
    whisper_trinity_amp_rel(instr, 0.1f);
    whisper_trinity_filter_amt(instr, 0.0f);

    /* vibrato */

    whisper_trinity_vibrato_rate(instr, 7.f);
    whisper_trinity_vibrato_depth(instr, 0.2);
}

static void trinity_bass(int instr)
{
    /* toggle first 2 oscillators */
    whisper_trinity_osc_toggle(instr, 0, 1);
    whisper_trinity_osc_toggle(instr, 1, 1);

    /* turn this one off */
    whisper_trinity_osc_toggle(instr, 2, 0);

    /* detune 2nd oscillator */
    whisper_trinity_osc_detune(instr, 1, 0.05);

    /* transpose first oscillotar one octave */
    whisper_trinity_osc_transpose(instr, 0, -12);

    /* all saw */
    whisper_trinity_osc_saw(instr, 0);
    whisper_trinity_osc_saw(instr, 1);

    /* filter */
    whisper_trinity_filter_moog(instr);
    whisper_trinity_cutoff(instr, 1000.f);
    whisper_trinity_res(instr, 0.01);
    whisper_trinity_filter_amt(instr, 1.0f);

    /* ADSR envelop */

    whisper_trinity_amp_atk(instr, 0.001f);
    whisper_trinity_amp_dec(instr, 0.2f);
    whisper_trinity_amp_sus(instr, 0.3f);
    whisper_trinity_amp_rel(instr, 0.07f);
    
    whisper_trinity_filter_atk(instr, 0.001f);
    whisper_trinity_filter_dec(instr, 0.3f);
    whisper_trinity_filter_sus(instr, 0.9f);
    whisper_trinity_filter_rel(instr, 0.07f);

    /* legato mode */
    whisper_trinity_legato(instr, 1);
    whisper_trinity_port_time(instr, 0.001);
    
    /* no vibrato */

    whisper_trinity_vibrato_rate(instr, 0.f);
    whisper_trinity_vibrato_depth(instr, 0.0);
}

static void trinity_saxophony(int instr)
{
    whisper_trinity_filter_butlp(instr);
    whisper_trinity_cutoff(instr, 1425.910034f);
    whisper_trinity_res(instr, 0.000000f);
    whisper_trinity_filt_dist(instr, 0.000000f);
    whisper_trinity_filter_atk(instr, 0.001000f);
    whisper_trinity_filter_dec(instr, 0.166299f);
    whisper_trinity_filter_sus(instr, 0.716535f);
    whisper_trinity_filter_rel(instr, 0.300000f);
    whisper_trinity_filter_amt(instr, 1.000000f);
    whisper_trinity_amp_atk(instr, 0.119071f);
    whisper_trinity_amp_dec(instr, 0.189913f);
    whisper_trinity_amp_sus(instr, 0.842520f);
    whisper_trinity_amp_rel(instr, 0.024614f);
    whisper_trinity_vibrato_rate(instr, 5.275590f);
    whisper_trinity_vibrato_depth(instr, 0.299213f);
    whisper_trinity_legato(instr, 1);
    whisper_trinity_port_time(instr, 0.007087f);
    whisper_trinity_osc_blsquare(instr, 0);
    whisper_trinity_osc_gain(instr, 0, 1.000000f);
    whisper_trinity_osc_transpose(instr, 0, 0.000000f);
    whisper_trinity_osc_detune(instr, 0, -0.050000f);
    whisper_trinity_osc_toggle(instr, 0, 1);
    whisper_trinity_osc_pulsewidth(instr, 0, 0.570630f);
    whisper_trinity_osc_saw(instr, 1);
    whisper_trinity_osc_gain(instr, 1, 1.000000f);
    whisper_trinity_osc_transpose(instr, 1, 0.000000f);
    whisper_trinity_osc_detune(instr, 1, 0.000000f);
    whisper_trinity_osc_toggle(instr, 1, 0);
    whisper_trinity_osc_pulsewidth(instr, 1, 0.500000f);
    whisper_trinity_osc_sine(instr, 2);
    whisper_trinity_osc_gain(instr, 2, 1.000000f);
    whisper_trinity_osc_transpose(instr, 2, 0.000000f);
    whisper_trinity_osc_detune(instr, 2, 0.070000f);
    whisper_trinity_osc_toggle(instr, 2, 0);
    whisper_trinity_osc_pulsewidth(instr, 2, 0.500000f);
}

static void trinity_whistle(int instr)
{
    whisper_trinity_filter_butlp(instr);
    whisper_trinity_cutoff(instr, 2291.180176f);
    whisper_trinity_res(instr, 0.000000f);
    whisper_trinity_filt_dist(instr, 0.000000f);
    whisper_trinity_filter_atk(instr, 0.100000f);
    whisper_trinity_filter_dec(instr, 0.100000f);
    whisper_trinity_filter_sus(instr, 0.500000f);
    whisper_trinity_filter_rel(instr, 0.300000f);
    whisper_trinity_filter_amt(instr, 0.000000f);
    whisper_trinity_amp_atk(instr, 0.001000f);
    whisper_trinity_amp_dec(instr, 0.001000f);
    whisper_trinity_amp_sus(instr, 1.000000f);
    whisper_trinity_amp_rel(instr, 0.001000f);
    whisper_trinity_vibrato_rate(instr, 7.086610f);
    whisper_trinity_vibrato_depth(instr, 0.299213f);
    whisper_trinity_legato(instr, 0);
    whisper_trinity_port_time(instr, 0.010000f);
    whisper_trinity_osc_triangle(instr, 0);
    whisper_trinity_osc_gain(instr, 0, 1.000000f);
    whisper_trinity_osc_transpose(instr, 0, 24.000000f);
    whisper_trinity_osc_detune(instr, 0, -0.050000f);
    whisper_trinity_osc_toggle(instr, 0, 1);
    whisper_trinity_osc_pulsewidth(instr, 0, 0.500000f);
    whisper_trinity_osc_noise(instr, 1);
    whisper_trinity_osc_gain(instr, 1, 0.000000f);
    whisper_trinity_osc_transpose(instr, 1, 0.000000f);
    whisper_trinity_osc_detune(instr, 1, 0.000000f);
    whisper_trinity_osc_toggle(instr, 1, 0);
    whisper_trinity_osc_pulsewidth(instr, 1, 0.500000f);
    whisper_trinity_osc_sine(instr, 2);
    whisper_trinity_osc_gain(instr, 2, 1.000000f);
    whisper_trinity_osc_transpose(instr, 2, 0.000000f);
    whisper_trinity_osc_detune(instr, 2, 0.070000f);
    whisper_trinity_osc_toggle(instr, 2, 0);
    whisper_trinity_osc_pulsewidth(instr, 2, 0.500000f);
}

static void trinity_woosh(int instr)
{
    whisper_trinity_filter_lpf18(instr);
    whisper_trinity_cutoff(instr, 2920.469971f);
    whisper_trinity_res(instr, 0.637795f);
    whisper_trinity_filt_dist(instr, 9.055120f);
    whisper_trinity_filter_atk(instr, 0.071843f);
    whisper_trinity_filter_dec(instr, 0.142685f);
    whisper_trinity_filter_sus(instr, 0.968504f);
    whisper_trinity_filter_rel(instr, 0.001000f);
    whisper_trinity_filter_amt(instr, 1.000000f);
    whisper_trinity_amp_atk(instr, 0.001000f);
    whisper_trinity_amp_dec(instr, 0.071843f);
    whisper_trinity_amp_sus(instr, 0.000000f);
    whisper_trinity_amp_rel(instr, 0.100000f);
    whisper_trinity_vibrato_rate(instr, 7.000000f);
    whisper_trinity_vibrato_depth(instr, 0.200000f);
    whisper_trinity_legato(instr, 0);
    whisper_trinity_port_time(instr, 0.010000f);
    whisper_trinity_osc_noise(instr, 0);
    whisper_trinity_osc_gain(instr, 0, 1.000000f);
    whisper_trinity_osc_transpose(instr, 0, 0.000000f);
    whisper_trinity_osc_detune(instr, 0, -0.050000f);
    whisper_trinity_osc_toggle(instr, 0, 1);
    whisper_trinity_osc_pulsewidth(instr, 0, 0.500000f);
    whisper_trinity_osc_triangle(instr, 1);
    whisper_trinity_osc_gain(instr, 1, 0.992126f);
    whisper_trinity_osc_transpose(instr, 1, 0.000000f);
    whisper_trinity_osc_detune(instr, 1, 0.000000f);
    whisper_trinity_osc_toggle(instr, 1, 0);
    whisper_trinity_osc_pulsewidth(instr, 1, 0.500000f);
    whisper_trinity_osc_sine(instr, 2);
    whisper_trinity_osc_gain(instr, 2, 1.000000f);
    whisper_trinity_osc_transpose(instr, 2, 0.000000f);
    whisper_trinity_osc_detune(instr, 2, 0.070000f);
    whisper_trinity_osc_toggle(instr, 2, 0);
    whisper_trinity_osc_pulsewidth(instr, 2, 0.500000f);
}

static void trinity_gameboy(int instr)
{
    whisper_trinity_filter_moog(instr);
    whisper_trinity_cutoff(instr, 10000.000000f);
    whisper_trinity_res(instr, 0.000000f);
    whisper_trinity_filt_dist(instr, 0.000000f);
    whisper_trinity_filter_atk(instr, 0.100000f);
    whisper_trinity_filter_dec(instr, 0.100000f);
    whisper_trinity_filter_sus(instr, 0.500000f);
    whisper_trinity_filter_rel(instr, 0.300000f);
    whisper_trinity_filter_amt(instr, 0.000000f);
    whisper_trinity_amp_atk(instr, 0.001000f);
    whisper_trinity_amp_dec(instr, 0.095457f);
    whisper_trinity_amp_sus(instr, 0.000000f);
    whisper_trinity_amp_rel(instr, 0.00500f);
    whisper_trinity_vibrato_rate(instr, 0.000000f);
    whisper_trinity_vibrato_depth(instr, 0.000000f);
    whisper_trinity_legato(instr, 0);
    whisper_trinity_port_time(instr, 0.010000f);
    whisper_trinity_osc_blsquare(instr, 0);
    whisper_trinity_osc_gain(instr, 0, 1.000000f);
    whisper_trinity_osc_transpose(instr, 0, 0.000000f);
    whisper_trinity_osc_detune(instr, 0, -0.050000f);
    whisper_trinity_osc_toggle(instr, 0, 1);
    whisper_trinity_osc_pulsewidth(instr, 0, 0.101102f);
    whisper_trinity_osc_saw(instr, 1);
    whisper_trinity_osc_gain(instr, 1, 1.000000f);
    whisper_trinity_osc_transpose(instr, 1, 0.000000f);
    whisper_trinity_osc_detune(instr, 1, 0.000000f);
    whisper_trinity_osc_toggle(instr, 1, 0);
    whisper_trinity_osc_pulsewidth(instr, 1, 0.500000f);
    whisper_trinity_osc_sine(instr, 2);
    whisper_trinity_osc_gain(instr, 2, 1.000000f);
    whisper_trinity_osc_transpose(instr, 2, 0.000000f);
    whisper_trinity_osc_detune(instr, 2, 0.070000f);
    whisper_trinity_osc_toggle(instr, 2, 0);
    whisper_trinity_osc_pulsewidth(instr, 2, 0.500000f);
}

void whisper_trinity_preset_setup(whisper_trinity *tri)
{
    unsigned int p;
    whisper_arg1 *presets;

    presets = whisper_trinity_preset_data(tri);

    for(p = 0; p < WHISPER_MAXPRESETS; p++) {
        presets[p] = trinity_default;
    }

    presets[1] = trinity_bass;
    presets[2] = trinity_saxophony;
    presets[3] = trinity_whistle;
    presets[4] = trinity_woosh;
    presets[5] = trinity_gameboy;
}


static void surgeon_default(int instr)
{
    surgeon_instr *surg;

    surg = whisper_surgeon_get_instr(instr);

    surgeon_algo(surg, 2);

    surgeon_op_mode(surg, 0, 1);
    surgeon_op_amp(surg, 0, 0.5);
    surgeon_op_scale(surg, 0, 1);
    
    surgeon_op_mode(surg, 1, 3);
    surgeon_op_index(surg, 1, 1);
    surgeon_op_scale(surg, 1, 1);
}

void whisper_surgeon_preset_setup(whisper_surgeon *surg)
{
    unsigned int p;
    whisper_arg1 *presets;

    presets = whisper_surgeon_preset_data(surg);

    for(p = 0; p < WHISPER_MAXPRESETS; p++) {
        presets[p] = surgeon_default;
    }
}

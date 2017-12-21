/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#ifndef WHISPER_DSP
#define WHISPER_DSP
#include "soundpipe.h"

#define WHISPER_MAXPRESETS 10
#define WHISPER_NTICKS 4
#define WHISPER_NTRACKS 5
#define WHISPER_NCLIPS 5

typedef struct whisper_trinity whisper_trinity;
typedef struct whisper_surgeon whisper_surgeon;
typedef struct surgeon_instr surgeon_instr; 
typedef struct whisper_mixer whisper_mixer; 
typedef struct whisper_effect whisper_effect; 
typedef struct whisper_track whisper_track;
typedef struct whisper_clip whisper_clip;
typedef struct whisper_db whisper_db;
typedef struct whisper_eyejam whisper_eyejam;
typedef struct whisper_xy whisper_xy;


typedef void (*whisper_arg0)(void);
typedef void (*whisper_arg1)(int);
typedef void (*whisper_arg2)(int, int);
typedef void (*whisper_compute)(SPFLOAT*);

typedef void (*whisper_effect_compute)(whisper_effect *, SPFLOAT*, SPFLOAT*, SPFLOAT*);
typedef void (*whisper_effect_init)(whisper_effect *, int sr);
typedef void (*whisper_effect_destroy)(whisper_effect *);

enum {
TRINITY_SINE,
TRINITY_SAW,
TRINITY_TRIANGLE,
TRINITY_SQUARE,
TRINITY_NOISE,
TRINITY_BLSAW,
TRINITY_BLSQUARE,
TRINITY_BLTRIANGLE,
TRINITY_MOOG = 0,
TRINITY_BUTLP,
TRINITY_BUTHP,
TRINITY_DIODE,
TRINITY_LPF18
};


EXPORT void whisper_set_drones(void);
void whisper_drones_init(int sr);
void whisper_drones_compute(SPFLOAT *output, unsigned int nframes);
void whisper_drones_tick(SPFLOAT *sample);
void whisper_drones_destroy(void);
void next_chord_please();
void noise_toggle();

void whisper_midway_init(int sr);
void whisper_midway_compute(SPFLOAT *output, unsigned int nframes);
void whisper_midway_tick(SPFLOAT *sample);
void whisper_midway_destroy(void);
void whisper_midway_please_recompile(void);
int whisper_midway_shouldi_recompile(void);
void whisper_midway_recompile(const char *str);

void whisper_midway_pset(int reg, SPFLOAT val);

EXPORT void whisper_set_midway(void);
EXPORT void whisper_midway_toggle_gun(void);
EXPORT void whisper_midway_toggle_beach(void);
EXPORT void whisper_midway_toggle_plane(void);
EXPORT void whisper_midway_toggle_bomb(void);

/* Theremin */

EXPORT void whisper_set_theremin(void);
void whisper_theremin_init(int sr);
void whisper_theremin_compute(SPFLOAT *output, unsigned int nframes);
void whisper_theremin_tick(SPFLOAT *sample);
void whisper_theremin_destroy(void);

EXPORT void whisper_theremin_set_x(float val);
EXPORT void whisper_theremin_set_y(float val);

/* Trinity */

EXPORT void whisper_set_trinity(void);
void whisper_trinity_init(int sr);
void whisper_trinity_compute(SPFLOAT *output, unsigned int nframes);
void whisper_trinity_tick(SPFLOAT *sample);
void whisper_trinity_tick_instr(int instr, SPFLOAT *sample);
void whisper_trinity_destroy(void);

EXPORT void whisper_trinity_noteon(int voice);
EXPORT void whisper_trinity_noteoff(int voice);
EXPORT void whisper_trinity_pitch(int voice, int nn);

EXPORT void whisper_trinity_voice_state(int instr, int voice, int state);

EXPORT void whisper_trinity_osc_sine(int instr, int osc);
EXPORT void whisper_trinity_osc_saw(int instr, int osc);
EXPORT void whisper_trinity_osc_square(int instr, int osc);
EXPORT void whisper_trinity_osc_triangle(int instr, int osc);
EXPORT void whisper_trinity_osc_blsaw(int instr, int osc);
EXPORT void whisper_trinity_osc_blsquare(int instr, int osc);
EXPORT void whisper_trinity_osc_bltriangle(int instr, int osc);
EXPORT void whisper_trinity_osc_noise(int instr, int osc);
EXPORT void whisper_trinity_osc_toggle(int instr, int osc, int state);
EXPORT void whisper_trinity_osc_detune(int instr, int osc, SPFLOAT detune);
EXPORT void whisper_trinity_osc_transpose(int instr, int osc, int trans);
EXPORT void whisper_trinity_osc_gain(int instr, int osc, SPFLOAT gain);
EXPORT void whisper_trinity_osc_pulsewidth(int instr, int osc, SPFLOAT width);

EXPORT void whisper_trinity_cutoff(int instr, SPFLOAT cutoff);
EXPORT void whisper_trinity_res(int instr, SPFLOAT res);
EXPORT void whisper_trinity_filt_dist(int instr, SPFLOAT dist);

EXPORT void whisper_trinity_default(int instr);

EXPORT void whisper_trinity_amp_atk(int instr, SPFLOAT val);
EXPORT void whisper_trinity_amp_dec(int instr, SPFLOAT val);
EXPORT void whisper_trinity_amp_sus(int instr, SPFLOAT val);
EXPORT void whisper_trinity_amp_rel(int instr, SPFLOAT val);

EXPORT void whisper_trinity_filter_atk(int instr, SPFLOAT val);
EXPORT void whisper_trinity_filter_dec(int instr, SPFLOAT val);
EXPORT void whisper_trinity_filter_sus(int instr, SPFLOAT val);
EXPORT void whisper_trinity_filter_rel(int instr, SPFLOAT val);

EXPORT void whisper_trinity_vgroup_init();
EXPORT void whisper_trinity_vgroup_preset(int instr, int preset);
EXPORT void whisper_trinity_multitimbral(int mode);
EXPORT void whisper_trinity_vgroup(int instr, int start, int size);

EXPORT void whisper_trinity_filter_lpf18(int instr);
EXPORT void whisper_trinity_filter_diode(int instr);
EXPORT void whisper_trinity_filter_moog(int instr);
EXPORT void whisper_trinity_filter_butlp(int instr);
EXPORT void whisper_trinity_filter_buthp(int instr);
EXPORT void whisper_trinity_filter_amt(int instr, SPFLOAT val);

EXPORT void whisper_trinity_legato(int instr, int mode);
EXPORT void whisper_trinity_port_time(int instr, SPFLOAT ptime);

EXPORT void whisper_trinity_vibrato_rate(int instr, SPFLOAT rate);
EXPORT void whisper_trinity_vibrato_depth(int instr, SPFLOAT depth);

EXPORT void whisper_trinity_set_master(int track);

void whisper_trinity_bind_track(int instr, int track);
EXPORT void whisper_trinity_preset(int instr, int preset);
EXPORT int whisper_trinity_preset_number(int instr);
void whisper_trinity_preset_setup(whisper_trinity *tri);
whisper_arg1* whisper_trinity_preset_data(whisper_trinity *tri);
EXPORT void whisper_trinity_preset_next(int instr);
EXPORT void whisper_trinity_preset_prev(int instr);

EXPORT void whisper_trinity_dump(int instr);

EXPORT const char * whisper_trinity_preset_name(int preset);

/* clip */

void wclip_create(whisper_clip **clip, 
        unsigned int max_length, 
        unsigned int maxvoices);
void wclip_set_length(whisper_clip *clip, unsigned int length);
void wclip_set_nvoices(whisper_clip *clip, unsigned int nvoices);
int wclip_get_nvoices(whisper_clip *clip);
void wclip_destroy(whisper_clip **clip);
int wclip_step(whisper_clip *clip);
int wclip_get_raw_value(whisper_clip *clip, int voice, int pos, int off);
int wclip_get_raw_note(whisper_clip *clip, int voice);
int wclip_get_note(whisper_clip *clip, int voice);
int wclip_get_arg(whisper_clip *clip, int voice, int arg);
int wclip_get_gate(whisper_clip *clip, int voice);
void wclip_set_gate(whisper_clip *clip, int pos, int voice, unsigned char gate);
void wclip_set_arg(whisper_clip *clip, int pos, int voice, int arg, unsigned char val);
void wclip_set_note(whisper_clip *clip, int pos, int voice, unsigned char nn);
void wclip_rewind(whisper_clip *clip);
void wclip_clear(whisper_clip *clip);
void wclip_clear_note(whisper_clip *clip, int voice, int pos);

EXPORT void whisper_clip_set_note(int track, int clip, int pos, int voice, int nn);
EXPORT void whisper_clip_set_length(int track, int clip, int length);
EXPORT void whisper_clip_set_gate(int track, int clip, int pos, int voice, int gate);
EXPORT void whisper_clip_set_arg(int track, int clip, int pos, int voice, int arg, int value);
EXPORT void whisper_clip_set_nvoices(int track, int clip, int nvoices);
EXPORT void whisper_clip_clear(int track, int clip);
EXPORT void whisper_clip_clear_note(int track, int clip, int voice, int pos);
EXPORT void whisper_clip_modified(int track, int clip);
whisper_clip * whisper_clip_get(int track, int clip);
unsigned char * wclip_get_data(whisper_clip *clip);
size_t wclip_get_data_size(whisper_clip *clip);
int wclip_get_maxvoices(whisper_clip *clip);
int wclip_get_nvoices(whisper_clip *clip);
int wclip_get_maxlength(whisper_clip *clip);
int wclip_get_length(whisper_clip *clip);
int wclip_get_row_id(whisper_clip *clip);

void wclip_set_maxvoices(whisper_clip *clip, unsigned int maxvoices);
void wclip_set_nvoices(whisper_clip *clip, unsigned int nvoices);
void wclip_set_maxlength(whisper_clip *clip, unsigned int maxlength);
void wclip_set_length(whisper_clip *clip, unsigned int length);
void wclip_set_row_id(whisper_clip *clip, int row_id);
void wclip_set_modflag(whisper_clip *clip);
int wclip_get_modflag(whisper_clip *clip);


/* track */

void whisper_tracks_init(int sr);
void whisper_tracks_init_instruments(int sr);
void whisper_tracks_destroy(void);
void whisper_tracks_tick(SPFLOAT *clock, SPFLOAT *sample);
void whisper_tracks_tick_track(SPFLOAT *clock, SPFLOAT *sample, int track);
EXPORT void whisper_schedule(int track, int clip);
EXPORT void whisper_tracks_loopmode(int track, int mode);
void whisper_tracks_set_init(int track, void (*fun)(int));
void whisper_tracks_set_tick(int track, void (*fun)(SPFLOAT*));
void whisper_tracks_set_destroy(int track, void (*fun)(void));
void whisper_tracks_set_noteon(int track, void (*fun)(int));
void whisper_tracks_set_noteoff(int track, void (*fun)(int));
void whisper_tracks_set_set_note(int track, void (*fun)(int, int));
EXPORT void whisper_tracks_gain(int track, SPFLOAT gain);
EXPORT void whisper_tracks_voice_offset(int track, int offset);
EXPORT int whisper_tracks_current_clip(int track);
EXPORT int whisper_tracks_next_clip(int track);
EXPORT int whisper_tracks_get_row_id(int track);
EXPORT void whisper_tracks_set_row_id(int track, int id);
EXPORT int whisper_tracks_get_clip_row_id(int track, int clip);
EXPORT void whisper_tracks_set_clip_row_id(int track, int clip, int id);
whisper_track * whisper_tracks_get_global_data();
whisper_track * whisper_tracks_get_track(int track);
whisper_clip * wtrack_get_clip(whisper_track *track, int clip);
SPFLOAT wtrack_get_gain(whisper_track *track);
int wtrack_get_loopmode(whisper_track *track);
EXPORT int whisper_tracks_get_loopmode(int track);
whisper_track * whisper_tracks_global_data();
int wtrack_get_meter(whisper_track *t);
EXPORT void whisper_tracks_turnoff(int track);
EXPORT double whisper_tracks_get_playhead(int track);
EXPORT int whisper_tracks_am_i_alive(int track);
EXPORT void whisper_track_modified(int track);
EXPORT int whisper_track_is_modified(int track);
void wtrack_modified(whisper_track *track);
int wtrack_is_modified(whisper_track *track);

/* EyeJam */

EXPORT void whisper_set_eyejam(void);
void whisper_eyejam_init(int sr);
void whisper_eyejam_compute(SPFLOAT *output, unsigned int nframes);
void whisper_eyejam_tick(SPFLOAT *sample);
void whisper_eyejam_tick_stereo(SPFLOAT *sampleL, SPFLOAT *sampleR);
void whisper_eyejam_destroy(void);

EXPORT void whisper_eyejam_demo_clips(void);
EXPORT void whisper_eyejam_metro(int mode);
EXPORT void whisper_eyejam_gain(SPFLOAT gain);
EXPORT float whisper_eyejam_gain_get();

EXPORT void whisper_eyejam_stress_test();

EXPORT float whisper_eyejam_pulse();

EXPORT float whisper_eyejam_tempo_get();
EXPORT void whisper_eyejam_tempo_set(float tempo);

EXPORT int whisper_eyejam_did_tempo_change();

void eyejam_populate_clip_with_args(int track, int clip, 
        const unsigned char *data, unsigned int len, unsigned int nargs);

whisper_eyejam * whisper_eyejam_global_data();
EXPORT void whisper_eyejam_play();
EXPORT void whisper_eyejam_stop();
EXPORT void whisper_eyejam_record();
EXPORT int whisper_eyejam_am_i_recording();

EXPORT void whisper_eyejam_xy_state_set(int state);
EXPORT int whisper_eyejam_xy_state_get();
EXPORT void whisper_eyejam_xy_pos_set(SPFLOAT x, SPFLOAT y);
EXPORT void whisper_eyejam_xy_toggle();
EXPORT int whisper_eyejam_xy_is_on();

EXPORT void whisper_eyejam_title_set(const char *title);
EXPORT const char * whisper_eyejam_title_get();


/* Soundpipe */

void whisper_sp_create(int sr);
int whisper_sp_is_created();
void whisper_sp_destroy();
sp_data * whisper_sp_data();

/* drumkit */

void whisper_drumkit_init(int sr);
void whisper_drumkit_compute(SPFLOAT *output, unsigned int nframes);
void whisper_drumkit_tick(SPFLOAT *sample);
void whisper_drumkit_destroy(void);
void whisper_drumkit_bind_track(int track);
void whisper_drumkit_load_sample(int sample, const char *path);
void whisper_drumkit_default_drums();
int whisper_drumkit_kit();
void whisper_drumkit_kit_set(int kit);

/* Surgeon */
void whisper_surgeon_setup();
void whisper_surgeon_bind_track(int instr, int track);
size_t surgeon_instr_size();
void surgeon_instr_init(sp_data *sp, surgeon_instr *surg);
void surgeon_instr_destroy(surgeon_instr *surg);
void surgeon_op_freq(surgeon_instr *surg, int osc, SPFLOAT freq);
void surgeon_op_amp(surgeon_instr *surg, int osc, SPFLOAT amp);
void surgeon_op_mode(surgeon_instr *surg, int osc, int mode);
void surgeon_op_index(surgeon_instr *surg, int osc, SPFLOAT index);
void surgeon_op_scale(surgeon_instr *surg, int osc, SPFLOAT scale);
void surgeon_op_atk(surgeon_instr *surg, int osc, SPFLOAT val);
void surgeon_op_dec(surgeon_instr *surg, int osc, SPFLOAT val);
void surgeon_op_sus(surgeon_instr *surg, int osc, SPFLOAT val);
void surgeon_op_rel(surgeon_instr *surg, int osc, SPFLOAT val);
void surgeon_op_amount(surgeon_instr *surg, int osc, SPFLOAT val);
void surgeon_freq(surgeon_instr *surg, int voice, SPFLOAT freq);
void surgeon_algo(surgeon_instr *surg, int algo);
void surgeon_noteon(surgeon_instr *surg, int voice);
void surgeon_noteoff(surgeon_instr *surg, int voice);
void surgeon_transpose(surgeon_instr *surg, int transpose);
void surgeon_setnote(surgeon_instr *surg, int voice, int nn);
void surgeon_compute_voice(sp_data *sp, surgeon_instr *ins, int v, SPFLOAT *o);
void surgeon_dump(surgeon_instr *ins);
void surgeon_preset(surgeon_instr *ins, int preset);
void surgeon_preset_list_init();
int surgeon_preset_number(surgeon_instr *ins);

EXPORT void whisper_surgeon_noteon(int instr, int voice);
EXPORT void whisper_surgeon_noteoff(int instr, int voice);
EXPORT void whisper_surgeon_setnote(int instr, int voice, int nn);
EXPORT void whisper_surgeon_algo(int instr, int algo);
EXPORT void whisper_surgeon_freq(int instr, float freq);

EXPORT void whisper_surgeon_op_freq(int instr, int osc, SPFLOAT freq);
EXPORT void whisper_surgeon_op_amp(int instr, int osc, SPFLOAT amp);
EXPORT void whisper_surgeon_op_mode(int instr, int osc, int mode);
EXPORT void whisper_surgeon_op_index(int instr, int osc, SPFLOAT index);
EXPORT void whisper_surgeon_op_scale(int instr, int osc, SPFLOAT scale);
EXPORT void whisper_surgeon_op_atk(int instr, int osc, SPFLOAT val);
EXPORT void whisper_surgeon_op_dec(int instr, int osc, SPFLOAT val);
EXPORT void whisper_surgeon_op_sus(int instr, int osc, SPFLOAT val);
EXPORT void whisper_surgeon_op_rel(int instr, int osc, SPFLOAT val);
EXPORT void whisper_surgeon_op_amount(int instr, int osc, SPFLOAT val);
whisper_arg1* whisper_surgeon_preset_data(whisper_surgeon *surg);
void whisper_surgeon_preset_setup(whisper_surgeon *surg);
EXPORT void whisper_surgeon_preset(int instr, int preset);
surgeon_instr * whisper_surgeon_get_instr(int instr);
EXPORT int whisper_surgeon_preset_number(int instr);
EXPORT void whisper_surgeon_dump(int instr);
EXPORT void whisper_surgeon_preset_next(int instr);
EXPORT void whisper_surgeon_preset_prev(int instr);
EXPORT const char * whisper_surgeon_preset_name(int preset);

/* Mixer and Effects */

void whisper_mixer_init(int sr);
void whisper_mixer_setup();
void whisper_mixer_destroy();
void whisper_mixer_tick(SPFLOAT *clock, SPFLOAT *L, SPFLOAT *R);
void whisper_mixer_send(int track, int send, SPFLOAT amt);
void whisper_mixer_reset();
void whisper_mixer_throw(int send, SPFLOAT val);

void whisper_effect_setup(whisper_effect *effect);
void whisper_effect_cb_init(whisper_effect *effect, whisper_effect_init init);
void whisper_effect_cb_destroy(whisper_effect *effect, whisper_effect_destroy destroy);
void whisper_effect_cb_compute(whisper_effect *effect, whisper_effect_compute compute);
void whisper_effect_data_set(whisper_effect *effect, void *ud);
void * whisper_effect_data_get(whisper_effect *effect);

void whisper_reverb_bind(whisper_effect *effect);
void whisper_delay_bind(whisper_effect *effect);
void whisper_delay_reset(whisper_effect *effect);

/* DB */

EXPORT void whisper_eyejam_db_open(const char *db);
EXPORT void whisper_eyejam_db_close();
EXPORT void whisper_eyejam_db_save_clip(int track, int clip, int id);
EXPORT void whisper_eyejam_db_load_clip(int track, int clip, int id);
EXPORT void whisper_eyejam_db_save_track(int track, int id);
EXPORT void whisper_eyejam_db_load_track(int track, int id);
EXPORT void whisper_eyejam_db_save_song(int id);
EXPORT void whisper_eyejam_db_load_song(int id);
EXPORT void whisper_eyejam_db_load_default();
EXPORT void whisper_eyejam_db_save_default();
EXPORT void whisper_eyejam_db_exec(const char *cmd);
EXPORT void whisper_eyejam_db_drumkit_slot(int slot);
EXPORT void whisper_eyejam_db_drumkit_sample(int pos, const char *path);
EXPORT int whisper_eyejam_db_songquery_begin();
EXPORT void whisper_eyejam_db_songquery_end();
EXPORT void whisper_eyejam_db_songquery_step();
EXPORT void whisper_eyejam_db_songquery_copy(char *str);
EXPORT void whisper_eyejam_db_songquery_print();

/* XY Control */

size_t whisper_xy_size();
void whisper_xy_init(whisper_xy *xy, sp_data *sp);
void whisper_xy_state(whisper_xy *xy, int state);
void whisper_xy_setpos(whisper_xy *xy, SPFLOAT x, SPFLOAT y);
void whisper_xy_getpos(float *x, float *y);
void whisper_xy_compute(whisper_xy *xy, 
    SPFLOAT *inL, SPFLOAT *inR, SPFLOAT *outL, SPFLOAT *outR);
void whisper_xy_cleanup(whisper_xy *xy);
whisper_xy * whisper_xy_global_data();
int whisper_xy_state_get(whisper_xy *xy);
void whisper_xy_state_toggle(whisper_xy *xy);
int whisper_xy_is_on(whisper_xy *xy);

/* Clip Editing */

EXPORT void whisper_eyejam_edit_init();
EXPORT void whisper_eyejam_edit_set_track(int track);
EXPORT int whisper_eyejam_edit_get_track();
EXPORT void whisper_eyejam_edit_set_clip(int clip);
EXPORT int whisper_eyejam_edit_get_clip();
EXPORT void whisper_eyejam_edit_set_base(int base);
EXPORT int whisper_eyejam_edit_get_base();
EXPORT void whisper_eyejam_edit_set_voice(int voice);
EXPORT int whisper_eyejam_edit_get_voice();
EXPORT void whisper_eyejam_edit_nextnote();
EXPORT void whisper_eyejam_edit_prevnote();
EXPORT void whisper_eyejam_edit_set_pos(int pos);
EXPORT int whisper_eyejam_edit_get_pos();
EXPORT void whisper_eyejam_edit_step_left();
EXPORT void whisper_eyejam_edit_step_right();
EXPORT void whisper_eyejam_edit_set_notelen(int notelen);
EXPORT int whisper_eyejam_edit_get_notelen();
EXPORT void whisper_eyejam_edit_insert(int nn);
EXPORT void whisper_eyejam_edit_clear();
EXPORT void whisper_eyejam_edit_remove();
EXPORT void whisper_eyejam_edit_transpose(int transpose);
EXPORT void whisper_eyejam_edit_nextclip();
EXPORT void whisper_eyejam_edit_prevclip();
EXPORT void whisper_eyejam_edit_play();
EXPORT int whisper_eyejam_edit_clip_length();
EXPORT void whisper_eyejam_edit_set_clip_length(int length);
EXPORT int whisper_eyejam_edit_clip_nvoices();
EXPORT int whisper_eyejam_edit_clip_note(int pos, int voice);
EXPORT int whisper_eyejam_edit_clip_gate(int pos, int voice);
EXPORT int whisper_eyejam_edit_isprevnote(int note);
EXPORT void whisper_eyejam_edit_stepsize_up();
EXPORT void whisper_eyejam_edit_stepsize_down();
EXPORT void whisper_eyejam_edit_stepsize_double();
EXPORT void whisper_eyejam_edit_stepsize_half();
EXPORT void whisper_eyejam_edit_voice_up();
EXPORT void whisper_eyejam_edit_voice_down();
EXPORT void whisper_eyejam_edit_track_next();
EXPORT void whisper_eyejam_edit_track_prev();
EXPORT double whisper_eyejam_edit_playhead();
EXPORT int whisper_eyejam_edit_playing();

#endif

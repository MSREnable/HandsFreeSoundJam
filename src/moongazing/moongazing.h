/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#ifndef MOONGAZING_H
#define MOONGAZING_H
#define NMOONS 4
#define NCOLORS 5

#ifndef MOONSCALE
#define MOONSCALE 1
#endif

#ifndef JAM_UI_DATA
#define JAM_UI_DATA
typedef struct jam_ui jam_ui;
#endif


/* data types */

typedef struct NVGcontext NVGcontext;
typedef struct mg_moon mg_moon;
typedef struct mg_audio mg_audio;
typedef struct mg_synth mg_synth;

/* top-level functions */

void mg_init(jam_ui *ui);
void mg_clean();
void mg_draw(NVGcontext *vg, double x, double y, double t);
void mg_bind_synth(mg_synth *synth);

/* moon */

void mg_moon_create(mg_moon **moon);
void mg_moon_destroy(mg_moon **moon);
void mg_moon_init(mg_moon *moon);
/* check for point intersection */
int mg_moon_hit(mg_moon *moon, double x, double y);
/* go to sleep mode */
void mg_moon_sleep(mg_moon *moon);
/* step in animation */
void mg_moon_step(mg_moon *moon, double delta);
void mg_moon_pos(mg_moon *moon, double *x, double *y);
double mg_moon_radius(mg_moon *moon);
double mg_moon_alpha(mg_moon *moon);
int mg_moon_color(mg_moon *moon);
int *mg_moon_pcolor(mg_moon *moon);
void mg_moon_init_colorhist();

/* randomly change location */
void mg_moon_relocate(mg_moon *moon);

/* window */

int mg_window_width(void);
int mg_window_height(void);
int mg_window_closing(void);


/* audio */

void mg_audio_create(mg_audio **mga);
void mg_audio_destroy(mg_audio **mga);
void mg_audio_init(mg_audio *mga);
void mg_audio_start(mg_audio *mga);
void mg_audio_stop(mg_audio *mga);
void mg_audio_set_synth(mg_audio *mga, mg_synth *synth);
int mg_audio_samplerate(mg_audio *mga);

/* synth */
void mg_synth_create(mg_synth **synth, int sr);
void mg_synth_destroy(mg_synth **synth);
void mg_synth_tick(mg_synth *synth, float *out);

/* cause a trigger event to happen on a particular object id */
void mg_synth_trigger(mg_synth *synth, int id);

/* bind colors to notes */
void mg_synth_bind_color(mg_synth *synth, int id, int *color);

/* change chords */
void mg_synth_change_chord(mg_synth *synth);

/* fade out alpha for smooth close */
float mg_time_fade(void);


#endif

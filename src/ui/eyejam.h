/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#ifndef FIDGET_EYEJAM_H
#define FIDGET_EYEJAM_H

#ifndef SCALING
#define SCALING 1
#endif

#define DEFAULT_DWELL 0.5

#define CONSTANT(X) ((X)*SCALING)

#define OPENSCREEN(name, MACRO) \
static void open_##name(jam_button *but, void *ud) \
{\
    jam_ui *ui;\
    ui = ud;\
    jam_ui_screen(ui, MACRO);\
}

typedef struct NVGcontext NVGcontext;

enum {
    EYEJAM_HIT,
    EYEJAM_MISS,
    EYEJAM_OVER,
    EYEJAM_OFF,
    EYEJAM_TICKING,
    EYEJAM_END,
    EYEJAM_PLEASE_REWIND,
    EYEJAM_TRIGGER
};

enum {
    JAM_LAUNCHER,
    JAM_EDIT,
    JAM_TOYS,
    JAM_MOONJAM,
    JAM_ARACHNOID,
    JAM_CONFIG,
    JAM_PRESETS,
    JAM_LOOPMODE,
};

typedef struct jam_audio jam_audio;
typedef struct jam_hit jam_hit;
typedef struct jam_timer jam_timer;
typedef struct jam_button jam_button;
typedef struct jam_ui jam_ui;
typedef struct jam_btnreg jam_btnreg;
typedef struct jam_piano jam_piano;
typedef struct jam_panel jam_panel;
typedef struct jam_pianoroll jam_pianoroll;
typedef struct jam_toys jam_toys;
typedef struct jam_config jam_config;
typedef struct jam_presets jam_presets;
typedef struct jam_loopmode jam_loopmode;

typedef void (*jam_button_cb)(jam_button*,void*);

/* Audio */

void jam_audio_init(jam_audio *jam);
void jam_audio_start(jam_audio *jam);
void jam_audio_stop(jam_audio *jam);
void jam_audio_create(jam_audio **jam);
void jam_audio_destroy(jam_audio **jam);

/* Hit/intersection tests */

void jam_hit_rect(jam_hit *hit, double x, double y, double w, double h);
void jam_hit_circ(jam_hit *hit, double x, double y, double rad);
int jam_hit_test(jam_hit *hit, double x, double y);
void jam_hit_set_pos(jam_hit *hit, double x, double y);
void jam_hit_set_size(jam_hit *hit, double w, double h);
void jam_hit_set_radius(jam_hit *hit, double rad);
size_t jam_hit_size();

/* Timer */

size_t jam_timer_size();
void jam_timer_reset(jam_timer *t);
void jam_timer_init(jam_timer *t);
void jam_timer_set_length(jam_timer *t, double len);
double jam_timer_alpha(jam_timer *t);
int jam_timer_increment(jam_timer *t, double delta);

/* Button */

size_t jam_button_size();
void jam_button_init(jam_button *but);
void jam_button_free(jam_button *but);
void jam_button_setsize(jam_button *but, double w, double h);
void jam_button_pos(jam_button *but, double x, double y);
void jam_button_reset(jam_button *but);
int jam_button_hit_test(jam_button *but, double x, double y);
int jam_button_interact(jam_button *but, double x, double y, double step);
int jam_button_step(jam_button *but, double delta);
void jam_button_draw(NVGcontext *vg, jam_button *but);
void jam_button_cb_hit(jam_button *but, jam_button_cb cb);
void jam_button_cb_over(jam_button *but, jam_button_cb cb);
void jam_button_cb_off(jam_button *but, jam_button_cb cb);
void jam_button_cb_trigger(jam_button *but, jam_button_cb cb);
void jam_button_color_normal(jam_button *but, int r, int g, int b);
void jam_button_color_normal_alt(jam_button *but, int clr, int r, int g, int b);
void jam_button_color_over(jam_button *but, int r, int g, int b);
void jam_button_color_select(jam_button *but, int r, int g, int b);
void jam_button_alt_color(jam_button *but, int mode);
void jam_button_scale(jam_button *but, double scale);
void jam_button_text(jam_button *but, const char *text);
void jam_button_data(jam_button *but, void *ud);
void jam_button_id(jam_button *but, int id);
int jam_button_id_get(jam_button *but);
void jam_button_dwell_set(jam_button *but, double time);

/* Main UI */

size_t jam_ui_size();
void jam_ui_init(jam_ui *ui);
void jam_ui_free(jam_ui *ui);
void jam_ui_step(NVGcontext *vg, jam_ui *ui, double x, double y, double delta);
void jam_ui_screen(jam_ui *ui, int screen);

/* Button region */

size_t jam_btnreg_size();
void jam_btnreg_init(jam_btnreg *reg);
void jam_btnreg_free(jam_btnreg *reg);
jam_button *jam_btnreg_button(jam_btnreg *reg, int n);
int jam_btnreg_hit_test(jam_btnreg *reg, double x, double y);
void jam_btnreg_interact(jam_btnreg *reg, double x, double y, double step);
void jam_btnreg_draw(NVGcontext *vg, jam_btnreg *reg, int cur, int next);
void jam_btnreg_pos(jam_btnreg *reg, double x, double y);
void jam_btnreg_layout(jam_btnreg *reg, int layout);
void jam_btnreg_set_track(jam_btnreg *reg, int track);
int jam_btnreg_track(jam_btnreg *reg);
void jam_btnreg_text(jam_btnreg *reg, const char *text);

/* Tobii Gaze */

void jam_tobii_setup();
void jam_tobii_destroy();
void jam_tobii_getxy(double *x, double *y);

/* Windowing */

int jam_win_width();
int jam_win_height();
void jam_close();


/* XY control */

void jam_xy_state_set(int state);
void jam_xy_pos_set(double x, double y);
int jam_xy_is_on();

/* Piano */

size_t jam_piano_size();
void jam_piano_init(jam_piano *piano, double x, double y);
void jam_piano_free(jam_piano *piano);
void jam_piano_interact(jam_piano *reg, double x, double y, double step);
void jam_piano_draw(NVGcontext *vg, jam_piano *reg);

/* Edit Panel */

size_t jam_panel_size();
void jam_panel_init(jam_panel *panel, jam_ui *ui, double x, double y);
void jam_panel_free(jam_panel *panel);
void jam_panel_interact(jam_panel *panel, double x, double y, double step);
void jam_panel_draw(NVGcontext *vg, jam_panel *panel);

/* Piano Roll */

size_t jam_pianoroll_size();
void jam_pianoroll_init(jam_pianoroll  *roll, double x, double y);
void jam_pianoroll_free(jam_pianoroll  *roll);
void jam_pianoroll_interact(jam_pianoroll *roll, double x, double y, double step);
void jam_pianoroll_draw(NVGcontext *vg, jam_pianoroll *roll);

/* Toys Window */

size_t jam_toys_size();
void jam_toys_init(jam_toys *toys, jam_ui *ui);
void jam_toys_free(jam_toys *toys);
void jam_toys_interact(jam_toys *toys, double x, double y, double step);
void jam_toys_draw(NVGcontext *vg, jam_toys *toys);
void jam_toys_step(NVGcontext *vg, jam_toys *toys, double x, double y, double step);

/* needed for some toys with alternate background color */
void jam_clear_color(float r, float g, float b);

/* Config Screen */

size_t jam_config_size();
void jam_config_init(jam_config *config, jam_ui *ui);
void jam_config_free(jam_config *config);
void jam_config_interact(jam_config *config, double x, double y, double step);
void jam_config_draw(NVGcontext *vg, jam_config *config);
void jam_config_step(NVGcontext *vg, jam_config *config, double x, double y, double step);

/* Presets Screen */

size_t jam_presets_size();
void jam_presets_init(jam_presets *presets, jam_ui *ui);
void jam_presets_free(jam_presets *presets);
void jam_presets_interact(jam_presets *presets, double x, double y, double step);
void jam_presets_draw(NVGcontext *vg, jam_presets *presets);
void jam_presets_step(NVGcontext *vg, jam_presets *presets, double x, double y, double step);

/* Loop Mode Screen */

size_t jam_loopmode_size();
void jam_loopmode_init(jam_loopmode *loopmode, jam_ui *ui);
void jam_loopmode_free(jam_loopmode *loopmode);
void jam_loopmode_interact(jam_loopmode *loopmode, double x, double y, double step);
void jam_loopmode_draw(NVGcontext *vg, jam_loopmode *loopmode);
void jam_loopmode_step(NVGcontext *vg, jam_loopmode *loopmode, double x, double y, double step);


/* labels */

const char *jam_track_label(int track);

#endif

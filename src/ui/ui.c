/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dsp/dsp.h"
#include "eyejam.h"
#include "moongazing/moongazing.h"
#include "nanovg.h"

#define NREGIONS 4

typedef struct {
    jam_ui *top;
    jam_btnreg *reg[NREGIONS];
    jam_button *close;
    jam_button *xy;
    jam_button *reset;
    jam_button *edit;
    jam_button *play;
    jam_button *stop;
    jam_button *tempo_up;
    jam_button *tempo_down;
    int centerw;
    int centerh;
} cliplauncher_ui;

typedef struct {
    jam_ui *top;
    jam_piano *piano;
    jam_panel *panel;
    jam_pianoroll *pianoroll;
} editor_ui;

struct jam_ui {
    jam_audio *audio;
    cliplauncher_ui launcher;
    editor_ui editor;
    int screen;
};

/* positions for button regions */
/* XY values are offsets from the center */
static double region_positions[] = {
/* top left */
-250, 150,
/* top right */
250, 150,
/* bottom left */
-250, -150,
/* bottom right */
250, -150
};

/* set up which tracks correspond to which region */
static int track_bindings[] = {
0, 1, 2, 4
};

/* region 0 callbacks */

static void reg_0_0(jam_button *but, void *ud)
{
    whisper_schedule(0, 0);
}

static void reg_0_1(jam_button *but, void *ud)
{
    whisper_schedule(0, 1);
}

static void reg_0_2(jam_button *but, void *ud)
{
    whisper_schedule(0, 2);
}

static void reg_0_3(jam_button *but, void *ud)
{
    whisper_schedule(0, 3);
}

static void reg_0_4(jam_button *but, void *ud)
{
    whisper_schedule(0, 4);
}

/* region 1 callbacks */

static void reg_1_0(jam_button *but, void *ud)
{
    whisper_schedule(1, 0);
}

static void reg_1_1(jam_button *but, void *ud)
{
    whisper_schedule(1, 1);
}

static void reg_1_2(jam_button *but, void *ud)
{
    whisper_schedule(1, 2);
}

static void reg_1_3(jam_button *but, void *ud)
{
    whisper_schedule(1, 3);
}

static void reg_1_4(jam_button *but, void *ud)
{
    whisper_schedule(1, 4);
}

/* region 2 callbacks */

static void reg_2_0(jam_button *but, void *ud)
{
    whisper_schedule(2, 0);
    whisper_schedule(3, 0);
}

static void reg_2_1(jam_button *but, void *ud)
{
    whisper_schedule(2, 1);
    whisper_schedule(3, 1);
}

static void reg_2_2(jam_button *but, void *ud)
{
    whisper_schedule(2, 2);
    whisper_schedule(3, 2);
}

static void reg_2_3(jam_button *but, void *ud)
{
    whisper_schedule(2, 3);
    whisper_schedule(3, 3);
}

static void reg_2_4(jam_button *but, void *ud)
{
    whisper_schedule(2, 4);
    whisper_schedule(3, 4);
}

/* region 3 callbacks */

static void reg_3_0(jam_button *but, void *ud)
{
    whisper_schedule(4, 0);
}

static void reg_3_1(jam_button *but, void *ud)
{
    whisper_schedule(4, 1);
}

static void reg_3_2(jam_button *but, void *ud)
{
    whisper_schedule(4, 2);
}

static void reg_3_3(jam_button *but, void *ud)
{
    whisper_schedule(4, 3);
}

static void reg_3_4(jam_button *but, void *ud)
{
    whisper_schedule(4, 4);
}

static void play_song(jam_button *but, void *ud)
{
    whisper_eyejam_play();
}

static void stop_song(jam_button *but, void *ud)
{
    whisper_eyejam_stop();
}

static void tempo_up(jam_button *but, void *ud)
{
    float tempo;
    tempo = whisper_eyejam_tempo_get();
    whisper_eyejam_tempo_set(tempo + 5.f);
}

static void tempo_down(jam_button *but, void *ud)
{
    float tempo;
    tempo = whisper_eyejam_tempo_get();
    whisper_eyejam_tempo_set(tempo - 5.f);
}

static void please_close(jam_button *but, void *ud)
{
    jam_close();
}

static void xy_toggle(jam_button *but, void *ud)
{
    /* whisper_eyejam_xy_toggle(); */
    jam_ui *ui;
    ui = ud;
    ui->screen = 2;
}

static void reset_track(jam_button *but, void *ud)
{
    /* turn off all tracks */
    int t;
    for(t = 0; t <5; t++) {
        whisper_tracks_turnoff(t);
    }
}

static void edit_screen(jam_button *but, void *ud)
{
    jam_ui *ui;
    ui = ud;
        
    jam_ui_screen(ui, 1);
}

static jam_button_cb event[] = 
{
    reg_0_0,
    reg_0_1,
    reg_0_2,
    reg_0_3,
    reg_0_4,
    
    reg_1_0,
    reg_1_1,
    reg_1_2,
    reg_1_3,
    reg_1_4,
    
    reg_2_0,
    reg_2_1,
    reg_2_2,
    reg_2_3,
    reg_2_4,
    
    reg_3_0,
    reg_3_1,
    reg_3_2,
    reg_3_3,
    reg_3_4,
};

size_t jam_ui_size()
{
    return sizeof(jam_ui);
}

static void editor_init(editor_ui *ui)
{
    int centerw;
    int centerh;
    int xpad;
    int butsize = CONSTANT(100);
    double space = CONSTANT((1024 - 800) / 7.0);
    double spacing = butsize + space; 

    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;

    /* x- padding is the remaining space of the piano width divided by 2 */
    /* button size = (size + pad) * 8 keys */
    xpad = (1024 - 880) / 2;
    
    ui->piano = malloc(jam_piano_size());
    jam_piano_init(ui->piano, 
        centerw - CONSTANT(512) + xpad
        , 
        /* place it so that y midline bisects keyboard */
        centerh - CONSTANT(110));

    /* set up edit panel */
    ui->panel = malloc(jam_panel_size());
    jam_panel_init(ui->panel, ui->top, 
        centerw - CONSTANT(512), 
        centerh + (CONSTANT(384) - (2*spacing - space)));

    /* set up piano roll */
    ui->pianoroll = malloc(jam_pianoroll_size());
    jam_pianoroll_init(ui->pianoroll, 
        centerw - CONSTANT(512 - 220 - 43), 
        centerh - CONSTANT(384));
}

static void editor_destroy(editor_ui *ui)
{
    jam_piano_free(ui->piano);
    free(ui->piano);
    jam_panel_free(ui->panel);
    free(ui->panel);
    jam_pianoroll_free(ui->pianoroll);
    free(ui->pianoroll);
}

static void editor_step(NVGcontext *vg, editor_ui *ui, 
    double x, double y, double delta) 
{
    jam_piano_interact(ui->piano, x, y, delta);
    jam_piano_draw(vg, ui->piano);
    jam_panel_interact(ui->panel, x, y, delta);
    jam_panel_draw(vg, ui->panel);
    jam_pianoroll_interact(ui->pianoroll, x, y, delta);
    jam_pianoroll_draw(vg, ui->pianoroll);
}

static void launcher_init(cliplauncher_ui *ui)
{
    int i;
    int b;
    int centerw;
    int centerh;
    jam_button *but;
    double hspacing;

    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;
    
    ui->centerw = centerw;
    ui->centerh = centerh;

    hspacing = CONSTANT(1024 / 4);
    /* subtract halfwidth of region, which should be ((100 + 10) * 3)/2  */
    centerw -= CONSTANT(330 / 2);
    /* subtract height of region, which should be ((100 + 10) * 2)/2 */
    centerh -= CONSTANT(220 / 2);
    for(i = 0; i < NREGIONS; i++) {
        ui->reg[i] = malloc(jam_btnreg_size());
        jam_btnreg_init(ui->reg[i]);
        jam_btnreg_pos(ui->reg[i], 
            centerw + CONSTANT(region_positions[2*i]), 
            centerh - CONSTANT(region_positions[2*i + 1]));
        if(i % 2 == 0) {
            jam_btnreg_layout(ui->reg[i], 0);
        } else {
            jam_btnreg_layout(ui->reg[i], 1);
        }

        for(b = 0; b < 5; b++) {
            but = jam_btnreg_button(ui->reg[i], b);
            jam_button_cb_trigger(but, event[i * 5 + b]);
        }

        jam_btnreg_set_track(ui->reg[i], track_bindings[i]);
    }

    jam_btnreg_text(ui->reg[0], "Melody");
    jam_btnreg_text(ui->reg[1], "Drums");
    jam_btnreg_text(ui->reg[2], "Chords");
    jam_btnreg_text(ui->reg[3], "Melody 2");

    /* make close button */
    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;

    ui->close = malloc(jam_button_size());
    jam_button_init(ui->close);
    jam_button_setsize(ui->close, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->close, 
        centerw - CONSTANT(512), 
        centerh + (CONSTANT(384 - 100)));
    jam_button_cb_trigger(ui->close, please_close);
    jam_button_text(ui->close, "Close");

    /* make XY button */
    ui->xy= malloc(jam_button_size());
    jam_button_init(ui->xy);
    jam_button_setsize(ui->xy, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->xy, 
        centerw - CONSTANT(512), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(ui->xy, xy_toggle);
    jam_button_text(ui->xy, "XY Mode");
    jam_button_data(ui->xy, ui->top);
    
    /* make reset button */
    ui->reset = malloc(jam_button_size());
    jam_button_init(ui->reset);
    jam_button_setsize(ui->reset, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->reset, 
        centerw + CONSTANT(512 - 100), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(ui->reset, reset_track);
    jam_button_text(ui->reset, "Reset");
    
    /* make edit button */
    ui->edit = malloc(jam_button_size());
    jam_button_init(ui->edit);
    jam_button_setsize(ui->edit, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->edit, 
        centerw + CONSTANT(512 - 100), 
        centerh + CONSTANT(384 - 100));
    jam_button_cb_trigger(ui->edit, edit_screen);
    jam_button_text(ui->edit, "Edit");
    jam_button_data(ui->edit, ui->top);

    /* make play button */
    ui->play = malloc(jam_button_size());
    jam_button_init(ui->play);
    jam_button_setsize(ui->play, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->play, 
        centerw - hspacing + CONSTANT(50), 
        centerh + CONSTANT(384 - 100));
    jam_button_cb_trigger(ui->play, play_song);
    jam_button_text(ui->play, "Play");
    jam_button_data(ui->play, ui->top);
    
    /* make stop button */
    ui->stop = malloc(jam_button_size());
    jam_button_init(ui->stop);
    jam_button_setsize(ui->stop, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->stop, 
        centerw + hspacing - CONSTANT(100) - CONSTANT(50), 
        centerh + CONSTANT(384 - 100));
    jam_button_cb_trigger(ui->stop, stop_song);
    jam_button_text(ui->stop, "Stop");
    jam_button_data(ui->stop, ui->top);
    
    /* tempo up */
    ui->tempo_up = malloc(jam_button_size());
    jam_button_init(ui->tempo_up);
    jam_button_setsize(ui->tempo_up, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->tempo_up, 
        centerw - hspacing + CONSTANT(50), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(ui->tempo_up, tempo_up);
    jam_button_text(ui->tempo_up, "+");
    jam_button_data(ui->tempo_up, ui->top);
    
    /* tempo down */
    ui->tempo_down = malloc(jam_button_size());
    jam_button_init(ui->tempo_down);
    jam_button_setsize(ui->tempo_down, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ui->tempo_down, 
        centerw + hspacing - CONSTANT(100) - CONSTANT(50), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(ui->tempo_down, tempo_down);
    jam_button_text(ui->tempo_down, "-");
    jam_button_data(ui->tempo_down, ui->top);
}

static void launcher_step(NVGcontext *vg, cliplauncher_ui *ui,
    double x, double y, double delta)
{
    int i;
    int clip;
    int next;
    int track;
    char buf[128];
    i = 0;
    for(i = 0; i < NREGIONS; i++) {
        track = jam_btnreg_track(ui->reg[i]);
        clip = whisper_tracks_current_clip(track);
        next = whisper_tracks_next_clip(track);
        jam_btnreg_interact(ui->reg[i], x, y, delta);
        jam_btnreg_draw(vg, ui->reg[i], clip, next);
    }
    jam_button_interact(ui->close, x, y, delta);
    jam_button_draw(vg, ui->close);
    jam_button_interact(ui->xy , x, y, delta);
    jam_button_draw(vg, ui->xy);
    jam_button_interact(ui->reset, x, y, delta);
    jam_button_draw(vg, ui->reset);
    jam_button_interact(ui->edit, x, y, delta);
    jam_button_draw(vg, ui->edit);
    jam_button_interact(ui->play, x, y, delta);
    jam_button_draw(vg, ui->play);
    jam_button_interact(ui->stop, x, y, delta);
    jam_button_draw(vg, ui->stop);
    jam_button_interact(ui->tempo_up, x, y, delta);
    jam_button_draw(vg, ui->tempo_up);
    jam_button_interact(ui->tempo_down, x, y, delta);
    jam_button_draw(vg, ui->tempo_down);

    /* write bpm */

    sprintf(buf, "Tempo: %g", whisper_eyejam_tempo_get());
    nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_TOP);
    nvgFontSize(vg, CONSTANT(28.0f));
    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgText(
        vg, 
        ui->centerw,    
        ui->centerh - CONSTANT(384),
        buf, 
        NULL
    );

}

static void launcher_destroy(cliplauncher_ui *ui)
{
    int i;
    /*
    for(i = 0; i < 4; i++) {
        jam_button_free(ui->but[i]);
        free(ui->but[i]);
    }
    */
    i = 0;
    for(i = 0; i < NREGIONS; i++) {
        jam_btnreg_free(ui->reg[i]);
        free(ui->reg[i]);
    }
    jam_button_free(ui->close);
    free(ui->close);
    jam_button_free(ui->xy);
    free(ui->xy);
    jam_button_free(ui->reset);
    free(ui->reset);
    jam_button_free(ui->edit);
    free(ui->edit);
    jam_button_free(ui->play);
    free(ui->play);
    jam_button_free(ui->stop);
    free(ui->stop);
    jam_button_free(ui->tempo_up);
    free(ui->tempo_up);
    jam_button_free(ui->tempo_down);
    free(ui->tempo_down);
}

void jam_ui_init(jam_ui *ui)
{
    jam_audio_create(&ui->audio);
    jam_audio_init(ui->audio);
    jam_audio_start(ui->audio);
    ui->launcher.top = ui;
    ui->editor.top = ui;
    launcher_init(&ui->launcher);
    editor_init(&ui->editor);
    ui->screen = 0;
    mg_init(ui);
}

void jam_ui_free(jam_ui *ui)
{
    launcher_destroy(&ui->launcher);
    editor_destroy(&ui->editor);
    jam_audio_stop(ui->audio);
    jam_audio_destroy(&ui->audio);
    mg_clean();
}

void jam_ui_step(NVGcontext *vg, jam_ui *ui, double x, double y, double delta)
{
    switch(ui->screen) {
        case 1:
            editor_step(vg, &ui->editor, x, y, delta);
            break;
        case 2:
            mg_draw(vg, x, y, delta);
            break;
        default:
            launcher_step(vg, &ui->launcher, x, y, delta);
            break;
    }
}

void jam_xy_state_set(int state)
{
    whisper_eyejam_xy_state_set(state);
}

void jam_xy_pos_set(double x, double y)
{
    /* clipping */
    if(x < 0) x = 0;
    else if(x > 1) x = 1;

    if(y < 0) y = 0;
    else if(y > 1) y = 1;

    /* flip y axis */
    y = 1 - y;
    whisper_eyejam_xy_pos_set(x, y);
}

int jam_xy_is_on()
{
    return whisper_eyejam_xy_is_on();
}

void jam_ui_screen(jam_ui *ui, int screen)
{
    ui->screen = screen;
}


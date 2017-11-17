/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dsp/dsp.h"
#include "eyejam.h"

struct jam_panel {
    jam_ui *top;
    jam_button *back;
    jam_button *clear;
    jam_button *remove;
    jam_button *play;
    jam_button *next_clip;
    jam_button *prev_clip;
    jam_button *octave_up;
    jam_button *octave_down;
    jam_button *step_up;
    jam_button *step_down;
    jam_button *voice_up;
    jam_button *voice_down;
    jam_button *next_track;
    jam_button *prev_track;
    jam_button *step_double;
    jam_button *step_half;
};

static void launcher_screen(jam_button *but, void *ud)
{
    jam_ui *ui;
    ui = ud;
        
    jam_ui_screen(ui, JAM_LAUNCHER);
}

static void clear_clip(jam_button *but, void *ud)
{
    whisper_eyejam_edit_clear();
    whisper_eyejam_edit_set_pos(0);
}

static void play_clip(jam_button *but, void *ud)
{
    whisper_eyejam_edit_play();
}

static void next_clip(jam_button *but, void *ud)
{
    whisper_eyejam_edit_nextclip();
    whisper_eyejam_edit_set_pos(0);
}

static void prev_clip(jam_button *but, void *ud)
{
    whisper_eyejam_edit_prevclip();
    whisper_eyejam_edit_set_pos(0);
}

static void octave_up(jam_button *but, void *ud)
{
    whisper_eyejam_edit_transpose(12);
}

static void octave_down(jam_button *but, void *ud)
{
    whisper_eyejam_edit_transpose(-12);
}

static void step_up(jam_button *but, void *ud)
{
    whisper_eyejam_edit_stepsize_up();
}

static void step_down(jam_button *but, void *ud)
{
    whisper_eyejam_edit_stepsize_down();
}

static void voice_up(jam_button *but, void *ud)
{
    whisper_eyejam_edit_voice_up();
}

static void voice_down(jam_button *but, void *ud)
{
    whisper_eyejam_edit_voice_down();
}

static void remove_note(jam_button *but, void *ud)
{
    whisper_eyejam_edit_remove();
}

static void next_track(jam_button *but, void *ud)
{
    int t;
    whisper_eyejam_edit_track_next();
    for(t = 0; t <5; t++) {
        whisper_tracks_turnoff(t);
    }
    whisper_eyejam_edit_set_pos(0);
}

static void prev_track(jam_button *but, void *ud)
{
    int t;
    whisper_eyejam_edit_track_prev();
    for(t = 0; t <5; t++) {
        whisper_tracks_turnoff(t);
    }
    whisper_eyejam_edit_set_pos(0);
}

static void step_double(jam_button *but, void *ud)
{
    whisper_eyejam_edit_stepsize_double();
}

static void step_half(jam_button *but, void *ud)
{
    whisper_eyejam_edit_stepsize_half();
}

size_t jam_panel_size()
{
    return sizeof(jam_panel);
}

void jam_panel_init(jam_panel *panel, jam_ui *ui, double x, double y)
{

    panel->top = ui;
    int butsize = CONSTANT(100);
    double space = CONSTANT((1024 - 800) / 7.0);
    double spacing = butsize + space; 


    /* play button */
    panel->play = malloc(jam_button_size());
    jam_button_init(panel->play);
    jam_button_setsize(panel->play, butsize, butsize);
    jam_button_pos(panel->play, x, y);
    jam_button_cb_trigger(panel->play, play_clip);
    jam_button_text(panel->play, "Play Clip");
    
    /* prev clip */
    panel->prev_clip = malloc(jam_button_size());
    jam_button_init(panel->prev_clip);
    jam_button_setsize(panel->prev_clip, butsize, butsize);
    jam_button_pos(panel->prev_clip, x + spacing, y);
    jam_button_cb_trigger(panel->prev_clip, prev_clip);
    jam_button_text(panel->prev_clip, "Prev Clip");
    
    /* next clip */
    panel->next_clip = malloc(jam_button_size());
    jam_button_init(panel->next_clip);
    jam_button_setsize(panel->next_clip, butsize, butsize);
    jam_button_pos(panel->next_clip, x + (2*spacing), y);
    jam_button_cb_trigger(panel->next_clip, next_clip);
    jam_button_text(panel->next_clip, "Next Clip");
    
    
    /* octave up */
    panel->octave_up = malloc(jam_button_size());
    jam_button_init(panel->octave_up);
    jam_button_setsize(panel->octave_up, butsize, butsize);
    jam_button_pos(panel->octave_up, x + (3*spacing), y);
    jam_button_cb_trigger(panel->octave_up, octave_up);
    jam_button_text(panel->octave_up, "+12");
    
    /* octave down */
    panel->octave_down = malloc(jam_button_size());
    jam_button_init(panel->octave_down);
    jam_button_setsize(panel->octave_down, butsize, butsize);
    jam_button_pos(panel->octave_down, x + (4*spacing), y);
    jam_button_cb_trigger(panel->octave_down, octave_down);
    jam_button_text(panel->octave_down, "-12");
    
    /* step_up */
    panel->step_up = malloc(jam_button_size());
    jam_button_init(panel->step_up);
    jam_button_setsize(panel->step_up, butsize, butsize);
    jam_button_pos(panel->step_up, x + (5*spacing), y);
    jam_button_cb_trigger(panel->step_up, step_up);
    jam_button_text(panel->step_up, "Step++");
    
    /* step_down */
    panel->step_down = malloc(jam_button_size());
    jam_button_init(panel->step_down);
    jam_button_setsize(panel->step_down, butsize, butsize);
    jam_button_pos(panel->step_down, x + (6*spacing), y);
    jam_button_cb_trigger(panel->step_down, step_down);
    jam_button_text(panel->step_down, "Step--");
    
    /* clear button */
    panel->clear = malloc(jam_button_size());
    jam_button_init(panel->clear);
    jam_button_setsize(panel->clear, butsize, butsize);
    jam_button_pos(panel->clear, x + (7*spacing), y);
    jam_button_cb_trigger(panel->clear, clear_clip);
    jam_button_text(panel->clear, "Clear Clip");
    jam_button_dwell_set(panel->clear, DEFAULT_DWELL * 1.5);
    
    /* Remove button */
    panel->remove = malloc(jam_button_size());
    jam_button_init(panel->remove);
    jam_button_setsize(panel->remove, butsize, butsize);
    jam_button_pos(panel->remove, x + (6*spacing), y + spacing);
    jam_button_cb_trigger(panel->remove, remove_note);
    jam_button_text(panel->remove, "Remove");
    jam_button_dwell_set(panel->remove, DEFAULT_DWELL * 1.2);
    
    /* Back Button */
    panel->back = malloc(jam_button_size());
    jam_button_init(panel->back);
    jam_button_setsize(panel->back, butsize, butsize);
    jam_button_pos(panel->back, x + 7 * spacing, y + spacing);
    jam_button_cb_trigger(panel->back, launcher_screen);
    jam_button_text(panel->back, "Launcher");
    jam_button_data(panel->back, panel->top);
    
    /* voice up */
    panel->voice_up = malloc(jam_button_size());
    jam_button_init(panel->voice_up);
    jam_button_setsize(panel->voice_up, butsize, butsize);
    jam_button_pos(panel->voice_up, x + 2*spacing, y + spacing);
    jam_button_cb_trigger(panel->voice_up, voice_up);
    jam_button_text(panel->voice_up, "Voice++");
    
    /* voice down */
    panel->voice_down = malloc(jam_button_size());
    jam_button_init(panel->voice_down);
    jam_button_setsize(panel->voice_down, butsize, butsize);
    jam_button_pos(panel->voice_down, x + 3*spacing, y + spacing);
    jam_button_cb_trigger(panel->voice_down, voice_down);
    jam_button_text(panel->voice_down, "Voice--");
    
    /* step double */
    panel->step_double = malloc(jam_button_size());
    jam_button_init(panel->step_double);
    jam_button_setsize(panel->step_double, butsize, butsize);
    jam_button_pos(panel->step_double, x + (4*spacing), y + spacing);
    jam_button_cb_trigger(panel->step_double, step_double);
    jam_button_text(panel->step_double, "Double");
    
    /* step half */
    panel->step_half = malloc(jam_button_size());
    jam_button_init(panel->step_half);
    jam_button_setsize(panel->step_half, butsize, butsize);
    jam_button_pos(panel->step_half, x + (5*spacing), y + spacing);
    jam_button_cb_trigger(panel->step_half, step_half);
    jam_button_text(panel->step_half, "Half");
   
    /* prev track */
    panel->prev_track = malloc(jam_button_size());
    jam_button_init(panel->prev_track);
    jam_button_setsize(panel->prev_track, butsize, butsize);
  
    jam_button_pos(panel->prev_track, x, y + spacing);
   
    jam_button_cb_trigger(panel->prev_track, prev_track);
    jam_button_text(panel->prev_track, "Prev Track");
    
    /* next track */
    panel->next_track = malloc(jam_button_size());
    jam_button_init(panel->next_track);
    jam_button_setsize(panel->next_track, butsize, butsize);
   
    jam_button_pos(panel->next_track, x + spacing, y + spacing);
  
    jam_button_cb_trigger(panel->next_track, next_track);
    jam_button_text(panel->next_track, "Next Track");
}

void jam_panel_free(jam_panel *panel)
{
    jam_button_free(panel->back);
    free(panel->back);
    jam_button_free(panel->clear);
    free(panel->clear);
    jam_button_free(panel->play);
    free(panel->play);
    jam_button_free(panel->next_clip);
    free(panel->next_clip);
    jam_button_free(panel->prev_clip);
    free(panel->prev_clip);
    jam_button_free(panel->octave_up);
    free(panel->octave_up);
    jam_button_free(panel->octave_down);
    free(panel->octave_down);
    jam_button_free(panel->step_up);
    free(panel->step_up);
    jam_button_free(panel->step_down);
    free(panel->step_down);
    jam_button_free(panel->remove);
    free(panel->remove);
    jam_button_free(panel->voice_up);
    free(panel->voice_up);
    jam_button_free(panel->voice_down);
    free(panel->voice_down);
    jam_button_free(panel->next_track);
    free(panel->next_track);
    jam_button_free(panel->prev_track);
    free(panel->prev_track);
    jam_button_free(panel->step_double);
    free(panel->step_double);
    jam_button_free(panel->step_half);
    free(panel->step_half);
}

void jam_panel_interact(jam_panel *panel, double x, double y, double step)
{
    jam_button_interact(panel->back, x, y, step);
    jam_button_interact(panel->clear, x, y, step);
    jam_button_interact(panel->play, x, y, step);
    jam_button_interact(panel->next_clip, x, y, step);
    jam_button_interact(panel->prev_clip, x, y, step);
    jam_button_interact(panel->octave_up, x, y, step);
    jam_button_interact(panel->octave_down, x, y, step);
    jam_button_interact(panel->step_up, x, y, step);
    jam_button_interact(panel->step_down, x, y, step);
    jam_button_interact(panel->remove, x, y, step);
    jam_button_interact(panel->voice_up, x, y, step);
    jam_button_interact(panel->voice_down, x, y, step);
    jam_button_interact(panel->next_track, x, y, step);
    jam_button_interact(panel->prev_track, x, y, step);
    jam_button_interact(panel->step_double, x, y, step);
    jam_button_interact(panel->step_half, x, y, step);
}

void jam_panel_draw(NVGcontext *vg, jam_panel *panel)
{
    jam_button_draw(vg, panel->back);
    jam_button_draw(vg, panel->clear);

    if(whisper_eyejam_edit_playing()) {
        jam_button_alt_color(panel->play, 1);
    } else {
        jam_button_alt_color(panel->play, 0);
    }

    jam_button_draw(vg, panel->play);
    jam_button_draw(vg, panel->next_clip);
    jam_button_draw(vg, panel->prev_clip);
    jam_button_draw(vg, panel->octave_up);
    jam_button_draw(vg, panel->octave_down);
    jam_button_draw(vg, panel->step_up);
    jam_button_draw(vg, panel->step_down);
    jam_button_draw(vg, panel->remove);
    jam_button_draw(vg, panel->voice_up);
    jam_button_draw(vg, panel->voice_down);
    jam_button_draw(vg, panel->next_track);
    jam_button_draw(vg, panel->prev_track);
    jam_button_draw(vg, panel->step_double);
    jam_button_draw(vg, panel->step_half);
}

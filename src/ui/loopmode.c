/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "dsp/dsp.h"
#include "eyejam.h"
#include "nanovg.h"

OPENSCREEN(launcher, JAM_CONFIG);

static void loopmode_off_0(jam_button *but, void *ud)
{
    whisper_tracks_loopmode(0, 0);
    whisper_track_modified(0);
}

static void loopmode_on_0(jam_button *but, void *ud)
{
    whisper_tracks_loopmode(0, 1);
    whisper_track_modified(0);
}

struct jam_loopmode {
    jam_ui *top;
    jam_button *on;
    jam_button *off;
    jam_button *launcher;
};

size_t jam_loopmode_size()
{
    return sizeof(jam_loopmode);
}

void jam_loopmode_init(jam_loopmode *loopmode, jam_ui *ui)
{
    int centerw;    
    int centerh;

    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;
    loopmode->top = ui;
    
    /* Off */
    loopmode->off= malloc(jam_button_size());
    jam_button_init(loopmode->off);
    jam_button_setsize(loopmode->off, CONSTANT(100), CONSTANT(100));
    jam_button_pos(loopmode->off, 
        centerw - (CONSTANT(150)), 
        centerh - CONSTANT(50));
    jam_button_cb_trigger(loopmode->off, loopmode_off_0); 
    jam_button_text(loopmode->off, "Off");
    jam_button_data(loopmode->off, loopmode->top);
    
    /* On */
    loopmode->on= malloc(jam_button_size());
    jam_button_init(loopmode->on);
    jam_button_setsize(loopmode->on, CONSTANT(100), CONSTANT(100));
    jam_button_pos(loopmode->on, 
        centerw + (CONSTANT(50)), 
        centerh - CONSTANT(50));
    jam_button_cb_trigger(loopmode->on, loopmode_on_0);
    jam_button_text(loopmode->on, "On");
    jam_button_data(loopmode->on, loopmode->top);
   
    /* Launcher */
    loopmode->launcher= malloc(jam_button_size());
    jam_button_init(loopmode->launcher);
    jam_button_setsize(loopmode->launcher, CONSTANT(100), CONSTANT(100));
    jam_button_pos(loopmode->launcher,
        centerw - CONSTANT(512), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(loopmode->launcher, open_launcher);
    jam_button_text(loopmode->launcher, "Config");
    jam_button_data(loopmode->launcher, loopmode->top);
}

void jam_loopmode_free(jam_loopmode *loopmode)
{
    jam_button_free(loopmode->off);
    free(loopmode->off);
    jam_button_free(loopmode->on);
    free(loopmode->on);
    jam_button_free(loopmode->launcher);
    free(loopmode->launcher);
}

void jam_loopmode_interact(jam_loopmode *loopmode, double x, double y, double step)
{

    if(whisper_tracks_get_loopmode(0)) {
        jam_button_alt_color(loopmode->off, 0);
        jam_button_alt_color(loopmode->on, 2);
    } else {
        jam_button_alt_color(loopmode->off, 2);
        jam_button_alt_color(loopmode->on, 0);
    }

    jam_button_interact(loopmode->off, x, y, step);
    jam_button_interact(loopmode->on, x, y, step);
    jam_button_interact(loopmode->launcher, x, y, step);
}

void jam_loopmode_draw(NVGcontext *vg, jam_loopmode *loopmode)
{
    jam_button_draw(vg, loopmode->off);
    jam_button_draw(vg, loopmode->on);
    jam_button_draw(vg, loopmode->launcher);
}

void jam_loopmode_step(NVGcontext *vg, jam_loopmode *loopmode, 
    double x, double y, double step)
{
    jam_loopmode_interact(loopmode, x, y, step);
    jam_loopmode_draw(vg, loopmode);
}


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
#define NUMTRACKS 5

#define LOOPMODE(TRACK) \
static void loopmode_off_##TRACK(jam_button *but, void *ud)\
{\
    whisper_tracks_loopmode(TRACK, 0);\
    whisper_track_modified(TRACK);\
}\
static void loopmode_on_##TRACK(jam_button *but, void *ud)\
{\
    whisper_tracks_loopmode(TRACK, 1);\
    whisper_track_modified(TRACK);\
}
#define LOOPENTRY(TRACK) loopmode_on_##TRACK, loopmode_off_##TRACK
OPENSCREEN(launcher, JAM_CONFIG);

LOOPMODE(0);
LOOPMODE(1);
LOOPMODE(2);
LOOPMODE(3);
LOOPMODE(4);

static const jam_button_cb bcb[] = {
    LOOPENTRY(0),
    LOOPENTRY(1),
    LOOPENTRY(2),
    LOOPENTRY(3),
    LOOPENTRY(4),
};

typedef struct {
    jam_button *on;
    jam_button *off;
} loopmode_entry;

struct jam_loopmode {
    jam_ui *top;
    loopmode_entry entry[NUMTRACKS];
    jam_button *launcher;
    int centerw;
    int centerh;
};

size_t jam_loopmode_size()
{
    return sizeof(jam_loopmode);
}

static void loopmode_entry_init(
    jam_loopmode *loopmode,
    loopmode_entry *ent,
    int offx,
    int offy,
    jam_button_cb cb_on,
    jam_button_cb cb_off
)
{

    /* Off */
    ent->off= malloc(jam_button_size());
    jam_button_init(ent->off);
    jam_button_setsize(ent->off, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ent->off, 
        loopmode->centerw - (offx + CONSTANT(100)), 
        loopmode->centerh - offy);
    jam_button_cb_trigger(ent->off, cb_off); 
    jam_button_text(ent->off, "Off");
    jam_button_data(ent->off, loopmode->top);
    
    /* On */
    ent->on= malloc(jam_button_size());
    jam_button_init(ent->on);
    jam_button_setsize(ent->on, CONSTANT(100), CONSTANT(100));
    jam_button_pos(ent->on, 
        loopmode->centerw + (offx), 
        loopmode->centerh - offy);
    jam_button_cb_trigger(ent->on, cb_on);
    jam_button_text(ent->on, "On");
    jam_button_data(ent->on, loopmode->top);
}

void jam_loopmode_init(jam_loopmode *loopmode, jam_ui *ui)
{
    int centerw;    
    int centerh;
    int i;

    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;
    loopmode->top = ui;
    loopmode->centerw = centerw;
    loopmode->centerh = centerh;

    for(i = 0; i < NUMTRACKS; i++) {
        loopmode_entry_init(loopmode, &loopmode->entry[i], 
            CONSTANT(50),
            CONSTANT(-250) + CONSTANT(150) * i,
            bcb[2 * i],
            bcb[2 * i + 1]);
    }

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

static void loopmode_entry_free(loopmode_entry *ent)
{
    jam_button_free(ent->off);
    free(ent->off);
    jam_button_free(ent->on);
    free(ent->on);
}

void jam_loopmode_free(jam_loopmode *loopmode)
{
    int i;
    for(i = 0; i < NUMTRACKS; i++) {
        loopmode_entry_free(&loopmode->entry[i]);
    }
    jam_button_free(loopmode->launcher);
    free(loopmode->launcher);
}

static void loopmode_entry_interact(loopmode_entry *ent, int track,
    double x, double y, double step)
{
    if(whisper_tracks_get_loopmode(track)) {
        jam_button_alt_color(ent->off, 1);
        jam_button_alt_color(ent->on, 0);
        jam_button_stroke_mode(ent->on, 1);
        jam_button_stroke_mode(ent->off, 0);
    } else {
        jam_button_alt_color(ent->off, 0);
        jam_button_alt_color(ent->on, 1);
        jam_button_stroke_mode(ent->on, 0);
        jam_button_stroke_mode(ent->off, 1);
    }

    jam_button_interact(ent->off, x, y, step);
    jam_button_interact(ent->on, x, y, step);
}

void jam_loopmode_interact(jam_loopmode *loopmode, double x, double y, double step)
{
    int i;
    for(i = 0; i < NUMTRACKS; i++) {
        loopmode_entry_interact(&loopmode->entry[i], i, x, y, step);
    }
    jam_button_interact(loopmode->launcher, x, y, step);
}

static void loopmode_entry_draw(NVGcontext *vg, loopmode_entry *ent)
{
    jam_button_draw(vg, ent->off);
    jam_button_draw(vg, ent->on);
}

void jam_loopmode_draw(NVGcontext *vg, jam_loopmode *loopmode)
{
    char buf[128];
    int i;
    nvgTextAlign(vg, NVG_ALIGN_RIGHT|NVG_ALIGN_CENTER);
    nvgFontSize(vg, CONSTANT(28.0f));
    
    nvgFillColor(vg, nvgRGB(255, 255, 255));


    for(i = 0; i < NUMTRACKS; i++) {
        nvgBeginPath(vg);
        sprintf(buf, "%s", jam_track_label(i));
        nvgText(
            vg, 
            loopmode->centerw - CONSTANT(180), 
            loopmode->centerh - CONSTANT(-300 + 150 * i),
            buf, 
            NULL
        );
        nvgFill(vg);
    }

    for(i = 0; i < NUMTRACKS; i++) {
    loopmode_entry_draw(vg, &loopmode->entry[i]);
    }

    jam_button_draw(vg, loopmode->launcher);
}

void jam_loopmode_step(NVGcontext *vg, jam_loopmode *loopmode, 
    double x, double y, double step)
{
    jam_loopmode_interact(loopmode, x, y, step);
    jam_loopmode_draw(vg, loopmode);
}


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

#define NBUTTONS 3
#define BSPACING CONSTANT(50)
#define OPENSCREEN(name, MACRO) \
static void open_##name(jam_button *but, void *ud) \
{\
    jam_ui *ui;\
    ui = ud;\
    jam_ui_screen(ui, MACRO);\
}

OPENSCREEN(presets, JAM_PRESETS);
OPENSCREEN(launcher, JAM_LAUNCHER);

typedef struct {
    const char *name;
    jam_button_cb cb;
} preset_button_config;

static preset_button_config bconfig[] = {
    {"Presets", open_presets},
    {"Loop Mode", open_launcher},
    {"Mixer", open_launcher},
};

struct jam_config {
    jam_ui *top;
    jam_button *but[NBUTTONS];
    jam_button *launcher;
};

size_t jam_config_size()
{
    return sizeof(jam_config);
}

void jam_config_init(jam_config *config, jam_ui *ui)
{
    int centerw;
    int centerh;
    int xpos;
    int ypos;
    int incr;
    int i;
    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;

    config->top = ui;

    ypos = centerh - CONSTANT(50);
    xpos = centerw - 
        ((CONSTANT(100) * NBUTTONS) + 
        (BSPACING * (NBUTTONS - 1))) / 2;

    incr = CONSTANT(100) + BSPACING;


    for(i = 0; i < NBUTTONS; i++) {
        config->but[i] = malloc(jam_button_size());
        jam_button_init(config->but[i]);
        jam_button_setsize(config->but[i], CONSTANT(100), CONSTANT(100));
        jam_button_pos(config->but[i], xpos, ypos);
        jam_button_cb_trigger(config->but[i], bconfig[i].cb);
        jam_button_text(config->but[i], bconfig[i].name);
        jam_button_data(config->but[i], config->top);
        xpos += incr;
    }
    
    config->launcher= malloc(jam_button_size());
    jam_button_init(config->launcher);
    jam_button_setsize(config->launcher, CONSTANT(100), CONSTANT(100));
    jam_button_pos(config->launcher,
        centerw - CONSTANT(512), 
        centerh - CONSTANT(50));
    jam_button_cb_trigger(config->launcher, open_launcher);
    jam_button_text(config->launcher, "Launcher");
    jam_button_data(config->launcher, config->top);
}

void jam_config_free(jam_config *config)
{
    int i;
    for(i = 0; i < NBUTTONS; i++) {
        jam_button_free(config->but[i]);
        free(config->but[i]);
    }
    jam_button_free(config->launcher);
    free(config->launcher);
}

void jam_config_interact(jam_config *config, double x, double y, double step)
{
    int i;
    for(i = 0; i < NBUTTONS; i++) {
        jam_button_interact(config->but[i], x, y, step);
    }
    jam_button_interact(config->launcher, x, y, step);
}

void jam_config_draw(NVGcontext *vg, jam_config *config)
{
    int i;
    for(i = 0; i < NBUTTONS; i++) {
        jam_button_draw(vg, config->but[i]);
    }
    jam_button_draw(vg, config->launcher);
}

void jam_config_step(NVGcontext *vg, jam_config *config, double x, double y, double step)
{
    jam_config_interact(config, x, y, step);
    jam_config_draw(vg, config);
}

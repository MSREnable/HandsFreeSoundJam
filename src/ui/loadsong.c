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

#define MAXSONGS 8

struct jam_loadsong {
    jam_ui *top;
    jam_button *config;
    int centerw;
    int centerh;
};

OPENSCREEN(config, JAM_CONFIG);

size_t jam_loadsong_size()
{
    return sizeof(jam_loadsong);
}
void jam_loadsong_init(jam_loadsong *loadsong, jam_ui *ui)
{
    int centerw;
    int centerh;
    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;

    loadsong->top = ui;
    loadsong->centerw = centerw;
    loadsong->centerh = centerh;
    
    /* Config */
    loadsong->config= malloc(jam_button_size());
    jam_button_init(loadsong->config);
    jam_button_setsize(loadsong->config, CONSTANT(100), CONSTANT(100));
    jam_button_pos(loadsong->config, 
        centerw - CONSTANT(512), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(loadsong->config, open_config);
    jam_button_text(loadsong->config, "Config");
    jam_button_data(loadsong->config, loadsong->top);
}

void jam_loadsong_free(jam_loadsong *loadsong)
{
    jam_button_free(loadsong->config);
    free(loadsong->config);
}

void jam_loadsong_interact(jam_loadsong *loadsong, double x, double y, double step)
{
    jam_button_interact(loadsong->config, x, y, step);
}

void jam_loadsong_draw(NVGcontext *vg, jam_loadsong *loadsong)
{
    jam_button_draw(vg, loadsong->config);
}

void jam_loadsong_step(NVGcontext *vg, jam_loadsong *loadsong, double x, double y, double step)
{
    jam_loadsong_interact(loadsong, x, y, step);
    jam_loadsong_draw(vg, loadsong);
}

void jam_loadsong_populate(jam_loadsong *loadsong)
{
    printf("Populating...\n");
}

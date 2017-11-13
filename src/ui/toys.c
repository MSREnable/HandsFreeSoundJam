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
#define NTOYS 2
#define RADIUS CONSTANT(250)

struct jam_toys {
    jam_ui *top;
    jam_button *launcher;
    jam_button *moonjam;
    jam_button *xy;
};

static void goto_launcher(jam_button *but, void *ud)
{
    jam_ui *ui;
    ui = ud;
        
    jam_ui_screen(ui, JAM_LAUNCHER);
}

static void xy_toggle(jam_button *but, void *ud)
{
    whisper_eyejam_xy_toggle();
}

static void goto_moonjam(jam_button *but, void *ud)
{
    jam_ui *ui;
    ui = ud;
        
    jam_ui_screen(ui, JAM_MOONJAM);
}

size_t jam_toys_size()
{
    return sizeof(jam_toys);
}

void jam_toys_init(jam_toys *toys, jam_ui *ui)
{
    int centerw;
    int centerh;
    int butsize = CONSTANT(100);
    double theta;
    double incr;
    double butoff;

    theta = M_PI / 2;
    butoff = butsize / 2.0;

    incr = (2 * M_PI) / NTOYS;
    
    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;
    toys->top = ui;

    /* Launcher button */
    toys->launcher = malloc(jam_button_size());
    jam_button_init(toys->launcher);
    jam_button_setsize(toys->launcher, butsize, butsize);
    jam_button_pos(toys->launcher, 
        centerw - CONSTANT(512), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(toys->launcher, goto_launcher);
    jam_button_text(toys->launcher, "Launcher");
    jam_button_data(toys->launcher, toys->top);
    
    /* Moonjam button */
    toys->moonjam = malloc(jam_button_size());
    jam_button_init(toys->moonjam);
    jam_button_setsize(toys->moonjam, butsize, butsize);
    jam_button_pos(toys->moonjam, 
        centerw - sin(theta)*RADIUS - butoff, 
        centerh - cos(theta)*RADIUS - butoff);
    jam_button_cb_trigger(toys->moonjam, goto_moonjam);
    jam_button_text(toys->moonjam, "MoonJam");
    jam_button_data(toys->moonjam, toys->top);
    theta += incr;
    
    /* XY button */
    toys->xy = malloc(jam_button_size());
    jam_button_init(toys->xy);
    jam_button_setsize(toys->xy, butsize, butsize);
    jam_button_pos(toys->xy, 
        centerw - sin(theta)*RADIUS - butoff, 
        centerh - cos(theta)*RADIUS - butoff);
    jam_button_cb_trigger(toys->xy, xy_toggle);
    jam_button_text(toys->xy, "XY Mode");
    jam_button_data(toys->xy, toys->top);
    theta += incr;
}

void jam_toys_free(jam_toys *toys)
{
    jam_button_free(toys->launcher);
    free(toys->launcher);
    jam_button_free(toys->moonjam);
    free(toys->moonjam);
    jam_button_free(toys->xy);
    free(toys->xy);
}

void jam_toys_interact(jam_toys *toys, double x, double y, double step)
{
    jam_button_interact(toys->launcher, x, y, step);
    jam_button_interact(toys->moonjam, x, y, step);
    jam_button_interact(toys->xy, x, y, step);
}

void jam_toys_draw(NVGcontext *vg, jam_toys *toys)
{
    jam_button_draw(vg, toys->launcher);
    jam_button_draw(vg, toys->moonjam);
    jam_button_draw(vg, toys->xy);
}

void jam_toys_step(NVGcontext *vg, jam_toys *toys, double x, double y, double step)
{
    jam_toys_interact(toys, x, y, step);
    jam_toys_draw(vg, toys);
}

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

static void open_presets(jam_button *but, void *ud)
{
    jam_ui *ui;
    ui = ud;
        
    jam_ui_screen(ui, JAM_LAUNCHER);
}

struct jam_presets {
    jam_ui *top;
    jam_button *presets;
};

size_t jam_presets_size()
{
    return sizeof(jam_presets);
}

void jam_presets_init(jam_presets *presets, jam_ui *ui)
{
    int centerw;
    int centerh;
    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;

    presets->top = ui;
    presets->presets = malloc(jam_button_size());
    jam_button_init(presets->presets);
    jam_button_setsize(presets->presets, CONSTANT(100), CONSTANT(100));
    jam_button_pos(presets->presets, 
        centerw - CONSTANT(50), 
        centerh - CONSTANT(50));
    jam_button_cb_trigger(presets->presets, open_presets);
    jam_button_text(presets->presets, "The Presets");
    jam_button_data(presets->presets, presets->top);
}

void jam_presets_free(jam_presets *presets)
{
    jam_button_free(presets->presets);
    free(presets->presets);
}

void jam_presets_interact(jam_presets *presets, double x, double y, double step)
{
    jam_button_interact(presets->presets, x, y, step);
}

void jam_presets_draw(NVGcontext *vg, jam_presets *presets)
{
    jam_button_draw(vg, presets->presets);
}

void jam_presets_step(NVGcontext *vg, jam_presets *presets, double x, double y, double step)
{
    jam_presets_interact(presets, x, y, step);
    jam_presets_draw(vg, presets);
}

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

struct jam_config {
    jam_ui *top;
    jam_button *presets;
};

size_t jam_config_size()
{
    return sizeof(jam_config);
}

void jam_config_init(jam_config *config, jam_ui *ui)
{
    int centerw;
    int centerh;
    centerw = jam_win_width() / 2;
    centerh = jam_win_height() / 2;

    config->top = ui;
    config->presets = malloc(jam_button_size());
    jam_button_init(config->presets);
    jam_button_setsize(config->presets, CONSTANT(100), CONSTANT(100));
    jam_button_pos(config->presets, 
        centerw - CONSTANT(50), 
        centerh - CONSTANT(50));
    jam_button_cb_trigger(config->presets, open_presets);
    jam_button_text(config->presets, "Presets");
    jam_button_data(config->presets, config->top);
}

void jam_config_free(jam_config *config)
{
    jam_button_free(config->presets);
    free(config->presets);
}

void jam_config_interact(jam_config *config, double x, double y, double step)
{
    jam_button_interact(config->presets, x, y, step);
}

void jam_config_draw(NVGcontext *vg, jam_config *config)
{
    jam_button_draw(vg, config->presets);
}

void jam_config_step(NVGcontext *vg, jam_config *config, double x, double y, double step)
{
    jam_config_interact(config, x, y, step);
    jam_config_draw(vg, config);
}

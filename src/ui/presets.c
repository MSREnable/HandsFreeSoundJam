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
        
    jam_ui_screen(ui, JAM_CONFIG);
}

static void trinity_preset_next(jam_button *but, void *ud)
{
    whisper_trinity_preset_next(0);
}

static void trinity_preset_prev(jam_button *but, void *ud)
{
    whisper_trinity_preset_prev(0);
}

struct jam_presets {
    jam_ui *top;
    jam_button *config;
    jam_button *prev;
    jam_button *next;
    int centerw;
    int centerh;
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
    presets->centerw = centerw;
    presets->centerh = centerh;

    /* Launcher */
    presets->config= malloc(jam_button_size());
    jam_button_init(presets->config);
    jam_button_setsize(presets->config, CONSTANT(100), CONSTANT(100));
    jam_button_pos(presets->config, 
        centerw - CONSTANT(512), 
        centerh - CONSTANT(384));
    jam_button_cb_trigger(presets->config, open_presets);
    jam_button_text(presets->config, "Config");
    jam_button_data(presets->config, presets->top);
    
    /* Previous */
    presets->prev= malloc(jam_button_size());
    jam_button_init(presets->prev);
    jam_button_setsize(presets->prev, CONSTANT(100), CONSTANT(100));
    jam_button_pos(presets->prev, 
        centerw - CONSTANT(120) - CONSTANT(50), 
        centerh - CONSTANT(50));
    jam_button_cb_trigger(presets->prev, trinity_preset_prev);
    jam_button_text(presets->prev, "-");
    jam_button_data(presets->prev, presets->top);
    
    /* Next */
    presets->next= malloc(jam_button_size());
    jam_button_init(presets->next);
    jam_button_setsize(presets->next, CONSTANT(100), CONSTANT(100));
    jam_button_pos(presets->next, 
        centerw + CONSTANT(120) - CONSTANT(50), 
        centerh - CONSTANT(50));
    jam_button_cb_trigger(presets->next, trinity_preset_next);
    jam_button_text(presets->next, "+");
    jam_button_data(presets->next, presets->top);
}

void jam_presets_free(jam_presets *presets)
{
    jam_button_free(presets->config);
    free(presets->config);
    jam_button_free(presets->next);
    free(presets->next);
    jam_button_free(presets->prev);
    free(presets->prev);
}

void jam_presets_interact(jam_presets *presets, double x, double y, double step)
{
    jam_button_interact(presets->config, x, y, step);
    jam_button_interact(presets->next, x, y, step);
    jam_button_interact(presets->prev, x, y, step);
}

void jam_presets_draw(NVGcontext *vg, jam_presets *presets)
{
    char buf[128];
    nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_TOP);
    nvgFontSize(vg, CONSTANT(28.0f));
    sprintf(buf, "Trinity 0 Preset: %d", whisper_trinity_preset_number(0));
    jam_button_draw(vg, presets->config);
    jam_button_draw(vg, presets->next);
    jam_button_draw(vg, presets->prev);
    
    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgBeginPath(vg);
    nvgText(
        vg, 
        presets->centerw, 
        presets->centerh - CONSTANT(60.f),
        buf, 
        NULL
    );
    nvgFill(vg);
}

void jam_presets_step(NVGcontext *vg, jam_presets *presets, double x, double y, double step)
{
    jam_presets_interact(presets, x, y, step);
    jam_presets_draw(vg, presets);
}

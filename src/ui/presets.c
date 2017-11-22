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

static void trinity_0_next(jam_button *but, void *ud)
{
    whisper_trinity_preset_next(0);
}

static void trinity_0_prev(jam_button *but, void *ud)
{
    whisper_trinity_preset_prev(0);
}

static void trinity_1_next(jam_button *but, void *ud)
{
    whisper_trinity_preset_next(1);
}

static void trinity_1_prev(jam_button *but, void *ud)
{
    whisper_trinity_preset_prev(1);
}

typedef struct {
    jam_presets *top;
    jam_button *prev;
    jam_button *next;
    int off_y;
} preset_select;

struct jam_presets {
    jam_ui *top;
    jam_button *config;
    preset_select trinity_0;
    preset_select trinity_1;
    int centerw;
    int centerh;
};

size_t jam_presets_size()
{
    return sizeof(jam_presets);
}

static void preset_select_init (
    jam_presets *presets, 
    preset_select *select,
    int off_x,
    int off_y,
    jam_button_cb prev,
    jam_button_cb next)
{
    
    /* Previous */
    select->prev= malloc(jam_button_size());
    jam_button_init(select->prev);
    jam_button_setsize(select->prev, CONSTANT(100), CONSTANT(100));
    jam_button_pos(select->prev, 
        presets->centerw - (off_x + CONSTANT(100)), 
        presets->centerh - off_y - CONSTANT(50));
    jam_button_cb_trigger(select->prev, prev);
    jam_button_text(select->prev, "-");
    jam_button_data(select->prev, presets->top);
    
    /* Next */
    select->next= malloc(jam_button_size());
    jam_button_init(select->next);
    jam_button_setsize(select->next, CONSTANT(100), CONSTANT(100));
    jam_button_pos(select->next, 
        presets->centerw + off_x, 
        presets->centerh - off_y - CONSTANT(50));
    jam_button_cb_trigger(select->next, next);
    jam_button_text(select->next, "+");
    jam_button_data(select->next, presets->top);
    select->off_y = off_y;
}

static void preset_select_free(preset_select *select)
{
    jam_button_free(select->next);
    free(select->next);
    jam_button_free(select->prev);
    free(select->prev);
}

static void preset_select_step(
    NVGcontext *vg, 
    preset_select *select, 
    double x,
    double y,
    double step)
{
    jam_button_interact(select->next, x, y, step);
    jam_button_interact(select->prev, x, y, step);
    jam_button_draw(vg, select->next);
    jam_button_draw(vg, select->prev);
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

    /* Trinity 0 */
    preset_select_init(presets, 
        &presets->trinity_0,
        CONSTANT(100),
        CONSTANT(140),
        trinity_0_prev,
        trinity_0_next);
    
    /* Trinity 1 */
    preset_select_init(presets, 
        &presets->trinity_1,
        CONSTANT(100),
        0,
        trinity_1_prev,
        trinity_1_next);
}

void jam_presets_free(jam_presets *presets)
{
    jam_button_free(presets->config);
    free(presets->config);
    preset_select_free(&presets->trinity_0);
    preset_select_free(&presets->trinity_1);
}

void jam_presets_interact(jam_presets *presets, double x, double y, double step)
{
    jam_button_interact(presets->config, x, y, step);
}

void jam_presets_draw(NVGcontext *vg, jam_presets *presets)
{
    char buf[128];
    nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_TOP);
    nvgFontSize(vg, CONSTANT(28.0f));
    jam_button_draw(vg, presets->config);
    
    nvgFillColor(vg, nvgRGB(255, 255, 255));
    
    nvgBeginPath(vg);
    sprintf(buf, "Trinity 0 Preset: %d", whisper_trinity_preset_number(0));
    nvgText(
        vg, 
        presets->centerw, 
        presets->centerh - (CONSTANT(60.f) + presets->trinity_0.off_y),
        buf, 
        NULL
    );
    nvgFill(vg);
    
    nvgBeginPath(vg);
    sprintf(buf, "Trinity 1 Preset: %d", whisper_trinity_preset_number(1));
    nvgText(
        vg, 
        presets->centerw, 
        presets->centerh - (CONSTANT(60.f) + presets->trinity_1.off_y),
        buf, 
        NULL
    );
    nvgFill(vg);
}

void jam_presets_step(NVGcontext *vg, jam_presets *presets, double x, double y, double step)
{
    jam_presets_interact(presets, x, y, step);
    jam_presets_draw(vg, presets);
    preset_select_step(vg, &presets->trinity_0, x, y, step);
    preset_select_step(vg, &presets->trinity_1, x, y, step);
}

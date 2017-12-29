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
#define YSTART 260 
#define YINCR 130

#define MAXSONGS 5

typedef struct {
    char title[41];
    int id;
    jam_button *but;
} loadsong_entry;

struct jam_loadsong {
    jam_ui *top;
    jam_button *config;
    int centerw;
    int centerh;
    loadsong_entry entry[MAXSONGS];
    int nsongs;
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
    int i;
    int cy;
    loadsong_entry *ent;
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

    loadsong->nsongs = 0;

    cy = YSTART;
    for(i = 0; i < MAXSONGS; i++) {
        ent = &loadsong->entry[i]; 
        ent->but= malloc(jam_button_size());
        jam_button_init(ent->but);
        jam_button_setsize(ent->but, CONSTANT(500), CONSTANT(100));
        jam_button_pos(ent->but, 
            centerw - CONSTANT(250), 
            //centerh - CONSTANT(cy) - CONSTANT(25));
            centerh - CONSTANT(cy) - CONSTANT(50));
        cy -= YINCR;
    }
}

void jam_loadsong_free(jam_loadsong *loadsong)
{
    int i;
    loadsong_entry *ent;
    jam_button_free(loadsong->config);
    free(loadsong->config);

    for(i = 0; i < MAXSONGS; i++) {
        ent = &loadsong->entry[i]; 
        jam_button_free(ent->but);
        free(ent->but);
    }
}

void jam_loadsong_interact(jam_loadsong *loadsong, double x, double y, double step)
{
    jam_button_interact(loadsong->config, x, y, step);
}

void jam_loadsong_draw(NVGcontext *vg, jam_loadsong *loadsong)
{
    int offy;
    int i;
    jam_button_draw(vg, loadsong->config);

    offy = YSTART;
    nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_TOP);
    nvgFontSize(vg, CONSTANT(48.0f));

    for(i = 0; i < loadsong->nsongs; i++) {
        jam_button_draw(vg, loadsong->entry[i].but);
        nvgFillColor(vg, nvgRGB(0, 0, 0));
        nvgBeginPath(vg);
        nvgText(
            vg, 
            loadsong->centerw, 
            loadsong->centerh - CONSTANT(offy) - CONSTANT(25),
            loadsong->entry[i].title, 
            NULL
        );
        nvgFill(vg);
        offy -= YINCR;
    }

}

void jam_loadsong_step(NVGcontext *vg, jam_loadsong *loadsong, double x, double y, double step)
{
    jam_loadsong_interact(loadsong, x, y, step);
    jam_loadsong_draw(vg, loadsong);
}

void jam_loadsong_populate(jam_loadsong *loadsong)
{
    int count;
    int i;
    loadsong_entry *entry;

    entry = loadsong->entry;
    count = whisper_eyejam_db_songquery_begin();
    printf("Populating...\n");
    
    if(count > MAXSONGS) count = MAXSONGS;

    loadsong->nsongs = count;
  
    for(i = 0; i < count; i++) {
        whisper_eyejam_db_songquery_step();
        whisper_eyejam_db_songquery_copy(entry[i].title);
        entry[i].id= whisper_eyejam_db_songquery_id();
        fprintf(stderr, "%d: %s\n", entry[i].id, entry[i].title);
    }

    whisper_eyejam_db_songquery_end();
}

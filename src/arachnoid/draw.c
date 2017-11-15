/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "nanovg.h"
#include "ui/eyejam.h"
#include "arachnoid.h"

static jam_button *exit_btn;

static void return_to_toys(jam_button *but, void *ud)
{
    jam_ui *ui;
    ui = ud;
        
    jam_ui_screen(ui, JAM_TOYS);
}


void arachnoid_step(NVGcontext *vg, double x, double y, double delta)
{
    int i;
    int rc;
    jam_clear_color((double)20/255, (double)26/255, (double)19/255);
    for(i = 0; i < 25; i++) {
        rc = arachnoid_circ_test(arachnoid_circ_get(i), x, y);
        if(rc == 0) {
            continue;
        } if(rc == 2){
            arachnoid_please_tick(i);
            break; 
        } else {
            break;
        }
    }

    nvgFillColor(vg, nvgRGB(221, 255, 85));
    for(i = 0; i < 25; i++) {
        arachnoid_circ_draw(vg, arachnoid_circ_get(i), delta);
    }

    jam_button_interact(exit_btn, x, y, delta);
    jam_button_draw(vg, exit_btn);
}

void arachnoid_init(jam_ui *ui)
{
    int count;
    int i;
    double theta;
    int win_width;
    int win_height;
    double centerw;
    double centerh;

    count = 0;

    win_width = jam_win_width();
    win_height = jam_win_height();
    
    centerw = (jam_win_width() / 2.0);
    centerh = (jam_win_height() / 2.0);

    arachnoid_circ_init(
        arachnoid_circ_get(count), 
        (win_width / 2), 
        (win_height / 2), 
        CONSTANT(84)
    );

    count++;

    theta = 2 * M_PI / 8.0;
    for(i = 0; i < 8; i++) {
        arachnoid_circ_init(
            arachnoid_circ_get(count), 
            (win_width / 2) + CONSTANT(180) * cos(theta * i) , 
            (win_height / 2) + CONSTANT(180) * sin(theta * i), 
            CONSTANT(60)
        );
        count++;
    }
    
    theta = 2 * M_PI / 16.0;
    for(i = 0; i < 16; i++) {
        arachnoid_circ_init(
            arachnoid_circ_get(count),
            (win_width / 2) + CONSTANT(300) * cos(theta * i) , 
            (win_height / 2) + CONSTANT(300) * sin(theta * i), 
            CONSTANT(45)
        );
        count++;
    }
    arachnoid_audio_init();

    /* make stop button */
    exit_btn = malloc(jam_button_size());
    jam_button_init(exit_btn);
    jam_button_setsize(exit_btn, CONSTANT(100), CONSTANT(100));
    jam_button_pos(exit_btn, 
        centerw - CONSTANT(512), 
        centerh + (CONSTANT(384 - 100)));
    jam_button_cb_trigger(exit_btn, return_to_toys);
    jam_button_text(exit_btn, "Return");
    jam_button_data(exit_btn, ui);
}

void arachnoid_clean()
{
    arachnoid_audio_clean();
    jam_button_free(exit_btn);
    free(exit_btn);
}

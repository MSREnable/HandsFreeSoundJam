#include <stdlib.h>
#include <stdio.h>
#include "dsp/dsp.h"
#include "eyejam.h"
#include "nanovg.h"

struct jam_btnreg {
    jam_button *but[5];
    jam_hit *hit;
    double x;
    double y;
    int state;
    int track;
    const char *text;
    int usetext;
};

size_t jam_btnreg_size()
{
    return sizeof(jam_btnreg);
}

void jam_btnreg_init(jam_btnreg *reg)
{
    int i;

    /* allocate and initialize memory for buttons */
    for(i = 0; i < 5; i++) {
        reg->but[i] = malloc(jam_button_size());
        jam_button_init(reg->but[i]);
        jam_button_setsize(reg->but[i], CONSTANT(100), CONSTANT(100));
    }

    /* allocate memory for intersection test */
    reg->hit = malloc(jam_hit_size());
   
    jam_btnreg_pos(reg, CONSTANT(0), CONSTANT(0));
    jam_btnreg_layout(reg, 0);
    reg->state = EYEJAM_MISS;

    /* set track to focus on track 0 by default */
    jam_btnreg_set_track(reg, 0);
    reg->usetext = 0;
}

void jam_btnreg_free(jam_btnreg *reg)
{
    int i;
    for(i = 0; i < 5; i++) {
        free(reg->but[i]);
    }
    free(reg->hit);
}

jam_button *jam_btnreg_button(jam_btnreg *reg, int n)
{
    return reg->but[n];
}

int jam_btnreg_hit_test(jam_btnreg *reg, double x, double y)
{
    int rc;

    rc = EYEJAM_MISS;

    return rc;
}

void jam_btnreg_interact(jam_btnreg *reg, double x, double y, double step)
{
    int rc;
    int i;

    rc = EYEJAM_MISS;

    /* first, check to see if there is intersection in the region */

    rc = jam_hit_test(reg->hit, x, y);
    reg->state = rc;
    switch(rc) {
        case EYEJAM_HIT:
        case EYEJAM_OVER:
            /* check for intersection on specific button */
            for(i = 0; i < 5; i++) {
                /* for now, go through them all. */
                /* TODO: consider having a case switch that ducks out early */
                rc = jam_button_interact(reg->but[i], x, y, step);
            }
            break;
        default:
            break;
    }

}

void jam_btnreg_draw(NVGcontext *vg, jam_btnreg *reg, int cur, int next)
{
    int i;
    float scale;
    for(i = 0; i < 5; i++) {
        /* constantly reset buttons if missed 
         * this semi-hack prevents button states from being frozen
         */
        if(reg->state == EYEJAM_MISS) {
            jam_button_reset(reg->but[i]);
        }
        jam_button_scale(reg->but[i], 1.0);
        if(i == next) {
            jam_button_alt_color(reg->but[i], 2);
            scale = whisper_eyejam_pulse();
            scale = 0.8 + 0.2 * scale;
            jam_button_scale(reg->but[i], scale);
        } else if(i == cur) {
            jam_button_alt_color(reg->but[i], 1);
        } else {
            jam_button_alt_color(reg->but[i], 0);
        }
        jam_button_draw(vg, reg->but[i]);
    }

    if(reg->usetext) {
        nvgTextAlign(vg, NVG_ALIGN_RIGHT|NVG_ALIGN_BOTTOM);
        nvgFontSize(vg, CONSTANT(28.0f));
        nvgFillColor(vg, nvgRGB(255, 255, 255));
        nvgText(vg, reg->x + CONSTANT(320.f), reg->y, reg->text, NULL);
    }
}

void jam_btnreg_pos(jam_btnreg *reg, double x, double y)
{
    /* default size is 3x2 grid of buttons */
    /* buttons 100px with 10px spacing */
    /* add spacing to bounds to detect pointeroff events */
    double off;
    off = CONSTANT(20);
    reg->x = x;
    reg->y = y;
    jam_hit_rect(reg->hit, reg->x, reg->y, CONSTANT(330)+off, CONSTANT(220)+off);
}

void jam_btnreg_layout(jam_btnreg *reg, int layout)
{
    switch(layout) {
        case 1:
            /* set layout of buttons to be:
             *        # # #
             *        # #
             *
             * make sure "head" points to button 1:
             *        2 1 0
             *        4 3
             *
             */ 

            jam_button_pos(reg->but[2], 
                reg->x, reg->y);
            jam_button_pos(reg->but[1], 
                reg->x + CONSTANT(110), reg->y);
            jam_button_pos(reg->but[0], 
                reg->x + CONSTANT(220), reg->y);
            jam_button_pos(reg->but[4], 
                reg->x, reg->y + CONSTANT(110));
            jam_button_pos(reg->but[3], 
                reg->x + CONSTANT(110), reg->y + CONSTANT(110));
            break;
        case 0:
        default: 
            /* set layout of buttons to be:
             *        # # # 
             *          # #
             *
             * with "head" pointing to 1:
             *        0 1 2 
             *          3 4
             * 
             */ 

            jam_button_pos(reg->but[0], 
                reg->x, reg->y);
            jam_button_pos(reg->but[1], 
                reg->x + CONSTANT(110), reg->y);
            jam_button_pos(reg->but[2], 
                reg->x + CONSTANT(220), reg->y);
            jam_button_pos(reg->but[3], 
                reg->x + CONSTANT(110), reg->y + CONSTANT(110));
            jam_button_pos(reg->but[4], 
                reg->x + CONSTANT(220), reg->y + CONSTANT(110));
            break;
    }
}

void jam_btnreg_set_track(jam_btnreg *reg, int track)
{
    reg->track = track;
}

int jam_btnreg_track(jam_btnreg *reg)
{
    return reg->track;
}

void jam_btnreg_text(jam_btnreg *reg, const char *text)
{
    reg->text = text;
    reg->usetext = 1;
}

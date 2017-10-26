#include <stdlib.h>
#include <stdio.h>
#include "dsp/dsp.h"
#include "eyejam.h"
#include "nanovg.h"

#define MAX_NOTES 4

enum {
NO_NOTE,
IN_NOTE
};

struct jam_pianoroll {
    jam_ui *top;
    int width;
    int height;
    int x;
    int y;
    int nlines;
    int base;
    NVGcolor bgcolor;
    NVGcolor fgcolor;
    NVGcolor darkcolor;
    NVGcolor notesel_clr[MAX_NOTES];
    NVGcolor note_clr[MAX_NOTES];
    jam_button *up;
    jam_button *down;
    jam_button *left;
    jam_button *right;
};

size_t jam_pianoroll_size()
{
    return sizeof(jam_pianoroll);
}

static void upwards(jam_button *but, void *ud)
{
    jam_pianoroll *roll;
    roll = ud;
    roll->base += 3;
}

static void downwards(jam_button *but, void *ud)
{
    jam_pianoroll *roll;
    roll = ud;
    roll->base -= 3;
}

static void leftwards(jam_button *but, void *ud)
{
    whisper_eyejam_edit_step_left();
}

static void rightwards(jam_button *but, void *ud)
{
    whisper_eyejam_edit_step_right();
}

void jam_pianoroll_init(jam_pianoroll *roll, double x, double y)
{
    double butsize = CONSTANT(85);
    double spacing;

    roll->x = x;
    roll->y = y;
    roll->width = CONSTANT(760);
    roll->height = CONSTANT(250);
    roll->bgcolor = nvgRGB(240, 240, 240);
    roll->fgcolor = nvgRGB(200, 200, 200);
    roll->darkcolor = nvgRGB(50, 50, 50); /* for beat gridlines */

    /* yellow */
    roll->note_clr[0] = nvgRGB(226, 187, 0); 
    roll->notesel_clr[0] = nvgRGB(255, 237, 155);
    /* blue */
    roll->note_clr[1] = nvgRGB(0, 135, 189); 
    roll->notesel_clr[1] = nvgRGB(117, 216, 255);
    /* red/pink */
    roll->note_clr[2] = nvgRGB(196, 2, 51); 
    roll->notesel_clr[2] = nvgRGB(252, 146, 174);
    /* green */
    roll->note_clr[3] = nvgRGB(0, 159, 107); 
    roll->notesel_clr[3] = nvgRGB(69, 255, 194);

    roll->nlines = 16;
    roll->base = 60;
   
    spacing = CONSTANT(1024) - roll->width;
    roll->width -= CONSTANT(15);
    roll->x += CONSTANT(15);

    butsize = spacing / 3.0;
    /* up button */
    roll->up = malloc(jam_button_size());
    jam_button_init(roll->up);
    jam_button_setsize(roll->up, butsize, butsize);
    jam_button_pos(roll->up, 
        x - 2*butsize,
        y);
    jam_button_cb_trigger(roll->up, upwards);
    jam_button_text(roll->up, "UP");
    jam_button_data(roll->up, roll);
    jam_button_dwell_set(roll->up, DEFAULT_DWELL * 0.8);
    
    /* down button */
    roll->down = malloc(jam_button_size());
    jam_button_init(roll->down);
    jam_button_setsize(roll->down, butsize, butsize);
    jam_button_pos(roll->down, 
        x - 2*butsize, 
        y + 2*butsize);
    jam_button_cb_trigger(roll->down, downwards);
    jam_button_text(roll->down, "DOWN");
    jam_button_data(roll->down, roll);
    jam_button_dwell_set(roll->down, DEFAULT_DWELL * 0.8);
    
    /* left button */
    roll->left = malloc(jam_button_size());
    jam_button_init(roll->left);
    jam_button_setsize(roll->left, butsize, butsize);
    jam_button_pos(roll->left, 
        x - spacing, 
        y + butsize);
    jam_button_cb_trigger(roll->left, leftwards);
    jam_button_text(roll->left, "LEFT");
    jam_button_data(roll->left, roll);
    jam_button_dwell_set(roll->left, DEFAULT_DWELL * 0.8);
    
    /* right button */
    roll->right = malloc(jam_button_size());
    jam_button_init(roll->right);
    jam_button_setsize(roll->right, butsize, butsize);
    jam_button_pos(roll->right, 
        x - butsize, 
        y + butsize);
    jam_button_cb_trigger(roll->right, rightwards);
    jam_button_text(roll->right, "RIGHT");
    jam_button_data(roll->right, roll);
    jam_button_dwell_set(roll->right, DEFAULT_DWELL * 0.8);
}

static void draw_note (
    NVGcontext *vg, 
    jam_pianoroll *roll, 
    int nn, 
    int steppos, 
    int dur, 
    double step_width, 
    double step_height,
    int chan 
)
{
    int ypos;
    int playhead;
    int curchan;

    ypos = (nn - roll->base);

    if(ypos < 0 || ypos >= roll->nlines) {
        return;
    }
    
    playhead = whisper_eyejam_edit_get_pos();
    curchan = whisper_eyejam_edit_get_voice();

    ypos = (roll->nlines - ypos) - 1;

    nvgBeginPath(vg);
    if(playhead == steppos && chan == curchan) {
        nvgFillColor(vg, roll->notesel_clr[chan]); 
    } else {
        nvgFillColor(vg, roll->note_clr[chan]); 
    }
    nvgStrokeColor(vg, roll->fgcolor);
    nvgRect(vg, 
        roll->x + step_width * steppos, 
        roll->y + ypos*step_height, 
        dur * step_width, 
        step_height);
    nvgFill(vg);
    nvgStroke(vg);
}

void jam_pianoroll_draw(NVGcontext *vg, jam_pianoroll *roll)
{
    double step_height;
    double step_width;
    double y;
    double y_2;
    double x;
    int i;
    int length;
    int mode;
    int dur;
    int p;
    int pos;
    int tmp;
    int note;
    int gate;
    int playhead;
    int notelen;
    int nvoices;
    int n;
    int curchan;
    double playpos;

    NVGcolor tmpcolor;
    mode = 0;
    step_height = (double)roll->height/roll->nlines;

    length = whisper_eyejam_edit_clip_length();
    notelen = whisper_eyejam_edit_get_notelen();
    step_width = (double)roll->width/length;

    nvgBeginPath(vg);
    nvgFillColor(vg, roll->bgcolor); 
    nvgRect(vg, roll->x, roll->y, roll->width, roll->height);
    nvgFill(vg);

    nvgStrokeColor(vg, roll->fgcolor);

    /* horizontal lines */
    for(i = 1; i <= roll->nlines; i++) {
        y = (i * step_height + roll->y);
        nvgBeginPath(vg);
        nvgMoveTo(vg, roll->x, y);
        nvgLineTo(vg, roll->x + roll->width, y);
        nvgStroke(vg);
    }
    
    /* vertical lines */
    for(i = 1; i < length; i++) {
        if(i % 4 == 0) {
            nvgStrokeColor(vg, roll->darkcolor);
        } else {
            nvgStrokeColor(vg, roll->fgcolor);
        }
        x = (i * step_width + roll->x);
        nvgBeginPath(vg);
        nvgMoveTo(vg, x, roll->y);
        nvgLineTo(vg, x, roll->y + roll->height);
        nvgStroke(vg);
    }

    nvoices = whisper_eyejam_edit_clip_nvoices();

    for(n = 0; n < nvoices; n++) {
        mode = NO_NOTE; 
        for(p = 0; p <= length; p++) {
            tmp = whisper_eyejam_edit_clip_note(p, n);
            gate = whisper_eyejam_edit_clip_gate(p, n);
            switch(mode) {
                case NO_NOTE:
                    if(gate != 0) {
                        mode = IN_NOTE;
                        dur = 1;
                        pos = p;
                        note = tmp;
                    }
                    break;
                case IN_NOTE:
                    if(gate == 0) {
                        mode = NO_NOTE;
                        draw_note(
                            vg, roll, note, 
                            pos, dur, step_width, step_height, n
                        );
                    } else {
                        if(whisper_eyejam_edit_isprevnote(tmp)) {
                            dur++;
                        } else {
                            draw_note(
                                vg, roll, note, 
                                pos, dur, step_width, step_height, n
                            );
                            note = tmp;
                            pos = p;
                            dur = 1;
                        }
                    }
                    break;
            }
        }
    }
   
  
    /* set up edit playhead position */
    playhead = whisper_eyejam_edit_get_pos();
    nvgStrokeColor(vg, nvgRGB(3, 255, 127));
    nvgBeginPath(vg);
    x = roll->x + playhead * step_width;
    y = roll->y;
    y_2 = roll->y + roll->height + CONSTANT(10);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x, y_2);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGB(3, 255, 127));
    nvgRect(vg, x - CONSTANT(5), y_2, CONSTANT(10), CONSTANT(10));
    nvgFill(vg);
   
    /* set up note block chunk */
    curchan = whisper_eyejam_edit_get_voice();
    nvgBeginPath(vg);
    tmpcolor = roll->note_clr[curchan];
    tmpcolor.a = 0.15f;
    nvgFillColor(vg, tmpcolor);
    nvgRect(vg, x, y, step_width * notelen, roll->height);
    nvgFill(vg);

    /* set up playback playhead position */
    if(whisper_eyejam_edit_playing()) {
        playpos = whisper_eyejam_edit_playhead();
        nvgStrokeColor(vg, nvgRGB(0, 0, 255));
        nvgBeginPath(vg);
        x = roll->x + playpos * step_width * length;
        y = roll->y;
        y_2 = roll->y + roll->height + CONSTANT(10);
        nvgMoveTo(vg, x, y);
        nvgLineTo(vg, x, y_2);
        nvgStroke(vg);
        nvgBeginPath(vg);
        nvgFillColor(vg, nvgRGB(0, 0, 255));
        nvgRect(vg, x - CONSTANT(5), y_2, CONSTANT(10), CONSTANT(10));
        nvgFill(vg);
    }

    jam_button_draw(vg, roll->up);
    jam_button_draw(vg, roll->down);
    jam_button_draw(vg, roll->left);
    jam_button_draw(vg, roll->right);
}

void jam_pianoroll_free(jam_pianoroll  *roll)
{
    jam_button_free(roll->up);
    free(roll->up);
    jam_button_free(roll->down);
    free(roll->down);
    jam_button_free(roll->left);
    free(roll->left);
    jam_button_free(roll->right);
    free(roll->right);
}

void jam_pianoroll_interact(jam_pianoroll *roll, double x, double y, double step)
{
    jam_button_interact(roll->up, x, y, step);
    jam_button_interact(roll->down, x, y, step);
    jam_button_interact(roll->left, x, y, step);
    jam_button_interact(roll->right, x, y, step);
}

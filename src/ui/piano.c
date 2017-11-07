/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dsp/dsp.h"
#include "eyejam.h"
#include "nanovg.h"

/* White key: x + 110, y = 110 */
/* Black Key: x + 55, y = 0 */
static int key_offsets[] = 
{
/* C */
0, 110,
/* C# */
55, 0,
/* D */
110, 110,
/* D# */
165, 0,
/* E */
220, 110,
/* F */
330, 110,
/* F# */
385, 0,
/* G */
440, 110,
/* G# */
495, 0,
/* A */
550, 110,
/* A# */
605, 0,
/* B */
660, 110,
/* C */
770, 110,
};

/* 0 = white; 1 = black */
static const char black_or_white_key[] =
{
/* C */
0,
/* C# */
1,
/* D */
0,
/* D# */
1,
/* E */
0,
/* F */
0,
/* F# */
1,
/* G */
0,
/* G# */
1,
/* A */
0,
/* A# */
1,
/* B */
0,
/* C */
0,
};

struct jam_piano {
    jam_button *keys[13];
    int id[13];
    double x;
    double y;
};

static void set_note(jam_piano *piano, int id, void *ud)
{
    whisper_eyejam_edit_insert(id);
}

static void hit_key(jam_button *bt, void *ud)
{
    jam_piano *pno;
    pno = ud;
    set_note(pno, jam_button_id_get(bt), ud);
}

size_t jam_piano_size()
{
    return sizeof(jam_piano);
}

static void create_key(jam_piano *piano, int id)
{
    jam_button *key;
    key =  malloc(jam_button_size());
    jam_button_init(key);
    jam_button_setsize(key, CONSTANT(100), CONSTANT(100));
    jam_button_pos(key, 
        piano->x + CONSTANT(key_offsets[2 * id]), 
        piano->y + CONSTANT(key_offsets[2 * id + 1]));
    jam_button_cb_trigger(key, hit_key);
    piano->id[id]=id;
    jam_button_data(key, &piano);
    if(black_or_white_key[id]) {
        /* black key */
        jam_button_color_normal(key, 128, 128, 128);
    } else {
        /* white key */
        jam_button_color_normal(key, 255, 255, 255);
    }
    jam_button_id(key, id);
    piano->keys[id] = key;
}

void jam_piano_init(jam_piano *piano, double x, double y)
{
    int k;
    piano->x = x;
    piano->y = y;
    for(k = 0; k < 13; k++) {
        create_key(piano, k);
    }
}

void jam_piano_free(jam_piano *piano)
{
    int k;
    for(k = 0; k < 13; k++) {
        jam_button_free(piano->keys[k]);
        free(piano->keys[k]);
    }
}

void jam_piano_interact(jam_piano *piano, double x, double y, double step)
{
    int k;
    for(k = 0; k < 13; k++) {
        jam_button_interact(piano->keys[k], x, y, step);
    }
}

void jam_piano_draw(NVGcontext *vg, jam_piano *piano)
{
    int k;
    int octave;
    int clip;
    int track;
    char buf[128];

    for(k = 0; k < 13; k++) {
        jam_button_draw(vg, piano->keys[k]);
    }


    /* write octave */
    octave = whisper_eyejam_edit_get_base();
    octave /= 12;
    octave--;

    sprintf(buf, "Octave: %d", octave);
    nvgTextAlign(vg, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
    nvgFontSize(vg, CONSTANT(28.0f));
    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgText(
        vg, 
        piano->x + CONSTANT(715), 
        piano->y,
        buf, 
        NULL
    );

    /* write clip */

    clip = whisper_eyejam_edit_get_clip();

    sprintf(buf, "Clip: %d", clip);
    nvgText(
        vg, 
        piano->x + CONSTANT(715), 
        piano->y + CONSTANT(28.f),
        buf, 
        NULL
    );
    
    /* write track */

    track = whisper_eyejam_edit_get_track();

    sprintf(buf, "Track: %d", track);
    nvgText(
        vg, 
        piano->x + CONSTANT(715), 
        piano->y + CONSTANT(56.f),
        buf, 
        NULL
    );

}

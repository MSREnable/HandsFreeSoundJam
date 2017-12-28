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

#define MAXSONGS 8

struct jam_loadsong {
    jam_ui *top;
    jam_button *but[MAXSONGS];
    jam_button *launcher;
};

size_t jam_loadsong_size()
{
    return sizeof(jam_loadsong);
}
void jam_loadsong_init(jam_loadsong *loadsong, jam_ui *ui)
{
    /* TODO: implement me! */
}

void jam_loadsong_free(jam_loadsong *loadsong)
{
    /* TODO: implement me! */
}

void jam_loadsong_interact(jam_loadsong *loadsong, double x, double y, double step)
{
    /* TODO: implement me! */
}

void jam_loadsong_draw(NVGcontext *vg, jam_loadsong *loadsong)
{
    /* TODO: implement me! */
}

void jam_loadsong_step(NVGcontext *vg, jam_loadsong *loadsong, double x, double y, double step)
{
    /* TODO: implement me! */
}

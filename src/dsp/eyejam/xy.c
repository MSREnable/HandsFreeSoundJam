/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include "dsp.h"

enum {
    XY_MODE_ON,
    XY_MODE_OFF,
    XY_MODE_FADEIN,
    XY_MODE_FADEOUT,
};

typedef struct {
    sp_scale *scaleX;
    sp_scale *scaleY;
    sp_wpkorg35 *filtL;
    sp_wpkorg35 *filtR;
} xy_filter;

typedef struct {
    sp_scale *scale;
    sp_delay *delayL;
    sp_delay *delayR;
} xy_delay;

struct whisper_xy {
    sp_data *sp;
    sp_port *portX;
    sp_port *portY;
    SPFLOAT x;
    SPFLOAT y;
    xy_filter xyfilt;
    xy_delay xydel;
    int state;
    unsigned int counter;
    unsigned int len;
};

static whisper_xy global_xy;

size_t whisper_xy_size()
{
    return sizeof(whisper_xy);
}

static void xy_delay_init(sp_data *sp, xy_delay *del, SPFLOAT deltime, 
    SPFLOAT fdbk_min, SPFLOAT fdbk_max)
{
    /* map Y to cutoff, 100Hz - 4000Hz */
    sp_scale_create(&del->scale);
    sp_scale_init(sp, del->scale);
    del->scale->min = fdbk_min; 
    del->scale->max = fdbk_max;

    sp_delay_create(&del->delayL);
    sp_delay_init(sp, del->delayL, deltime);
    sp_delay_create(&del->delayR);
    sp_delay_init(sp, del->delayR, deltime);
}

static void xy_delay_destroy(xy_delay *del)
{
    sp_scale_destroy(&del->scale);
    sp_delay_destroy(&del->delayL);
    sp_delay_destroy(&del->delayR);
}

void xy_delay_compute(sp_data *sp, xy_delay *del,
    SPFLOAT *val,
    SPFLOAT *inL, SPFLOAT *inR,
    SPFLOAT *outL, SPFLOAT *outR)
{
    SPFLOAT s_fdbk;
    s_fdbk = 0;
    sp_scale_compute(sp, del->scale, val, &s_fdbk);

    del->delayL->feedback = s_fdbk;
    del->delayR->feedback = s_fdbk;

    sp_delay_compute(sp, del->delayL, inL, outL);
    sp_delay_compute(sp, del->delayR, inR, outR);

    *outL = 0.8f * *inL + 0.2f * *outL;
    *outR = 0.8f * *inR + 0.2f * *outR;
}

static void xy_filter_init(sp_data *sp, xy_filter *xy,
    SPFLOAT cutoff_min, SPFLOAT cutoff_max, 
    SPFLOAT res_min, SPFLOAT res_max)
{

    /* set up filter and scale for filter */
    sp_wpkorg35_create(&xy->filtL);
    sp_wpkorg35_init(sp, xy->filtL);
    
    sp_wpkorg35_create(&xy->filtR);
    sp_wpkorg35_init(sp, xy->filtR);

    /* map X to resonance, 0.5 - 1.9 */
    sp_scale_create(&xy->scaleX);
    sp_scale_init(sp, xy->scaleX);
    xy->scaleX->min = res_min;
    xy->scaleX->max = res_max; 

    /* map Y to cutoff, 100Hz - 4000Hz */
    sp_scale_create(&xy->scaleY);
    sp_scale_init(sp, xy->scaleY);
    xy->scaleY->min = cutoff_min; 
    xy->scaleY->max = cutoff_max;
}

static void xy_filter_destroy(xy_filter *xy)
{
    sp_wpkorg35_destroy(&xy->filtL);
    sp_wpkorg35_destroy(&xy->filtR);
    sp_scale_destroy(&xy->scaleX);
    sp_scale_destroy(&xy->scaleY);
}

void whisper_xy_init(whisper_xy *xy, sp_data *sp)
{
    xy->sp = sp;

    /* set up smoothing filters */
    sp_port_create(&xy->portX);
    sp_port_init(xy->sp, xy->portX, 0.02f);
    sp_port_create(&xy->portY);
    sp_port_init(xy->sp, xy->portY, 0.02f);
    whisper_xy_setpos(xy, 0.f, 0.f);

    xy_filter_init(xy->sp, &xy->xyfilt, 100.f, 4000.f, 0.4f, 1.9f);
    xy_delay_init(xy->sp, &xy->xydel, 0.15f, 0.0f, 0.99f);

    whisper_xy_state(xy, XY_MODE_OFF);

    xy->len = xy->sp->sr * 0.1f;
    xy->counter = 0;
}

void xy_filter_compute(sp_data *sp, xy_filter *xy,
    SPFLOAT *x, SPFLOAT *y, 
    SPFLOAT *inL, SPFLOAT *inR,
    SPFLOAT *outL, SPFLOAT *outR)
{
    SPFLOAT s_scaleX;
    SPFLOAT s_scaleY;
    sp_scale_compute(sp, xy->scaleX, x, &s_scaleX);
    sp_scale_compute(sp, xy->scaleY, y, &s_scaleY);
    xy->filtL->cutoff = s_scaleY;
    xy->filtL->res = s_scaleX;
    xy->filtR->cutoff = s_scaleY;
    xy->filtR->res = s_scaleX;

    sp_wpkorg35_compute(sp, xy->filtL, inL, outL);
    sp_wpkorg35_compute(sp, xy->filtR, inR, outR);
}

void whisper_xy_state(whisper_xy *xy, int state)
{
    xy->state = state;
}

void whisper_xy_setpos(whisper_xy *xy, SPFLOAT x, SPFLOAT y)
{
    xy->x = x;
    xy->y = y;
}

void whisper_xy_compute(whisper_xy *xy, 
    SPFLOAT *inL, SPFLOAT *inR, SPFLOAT *outL, SPFLOAT *outR)
{
    SPFLOAT s_portX;
    SPFLOAT s_portY;
    SPFLOAT tmp_inL;
    SPFLOAT tmp_inR;
    SPFLOAT tmp_outL;
    SPFLOAT tmp_outR;
    SPFLOAT pos;
    

    *outL = *inL;
    *outR = *inR;

    tmp_outL = 0;
    tmp_outR = 0;
    pos = 0;

    if(xy->state == XY_MODE_ON ||
        xy->state == XY_MODE_FADEIN ||
        xy->state == XY_MODE_FADEOUT) {
        sp_port_compute(xy->sp, xy->portX, &xy->x, &s_portX);
        sp_port_compute(xy->sp, xy->portY, &xy->y, &s_portY);
        xy_filter_compute(xy->sp, &xy->xyfilt, &s_portX, &s_portY, inL, inR, 
            &tmp_inL, &tmp_inR);
        xy_delay_compute(xy->sp, &xy->xydel, &s_portX,
            &tmp_inL, &tmp_inR, &tmp_outL, &tmp_outR);
            
        pos = 1.; 

        if(xy->state == XY_MODE_FADEIN) {
            /* go from 0 to 1 */
            pos = (SPFLOAT) xy->counter / xy->len;
            /* counters go backwards, so reverse by subtracting 1 */
            pos = 1 - pos;

            xy->counter--;
            if(xy->counter == 0) xy->state = XY_MODE_ON;
        } else if(xy->state == XY_MODE_FADEOUT) {

            /* go from 1 to 0 */
            pos = (SPFLOAT) xy->counter / xy->len;

            xy->counter--;
            if(xy->counter == 0) xy->state = XY_MODE_OFF;
        } 

        *outL = tmp_outL * pos + *inL * (1.f - pos);
        *outR = tmp_outR * pos + *inR * (1.f - pos);
    }
}

void whisper_xy_cleanup(whisper_xy *xy)
{
    xy_filter_destroy(&xy->xyfilt);
    xy_delay_destroy(&xy->xydel);
    sp_port_destroy(&xy->portX);
    sp_port_destroy(&xy->portY);
}

whisper_xy * whisper_xy_global_data()
{
    return &global_xy;
}

int whisper_xy_state_get(whisper_xy *xy)
{
    return xy->state;
}

void whisper_xy_state_toggle(whisper_xy *xy)
{
    if(xy->state == XY_MODE_OFF || xy->state == XY_MODE_FADEOUT) {
        xy->state = XY_MODE_FADEIN;
    } else if(xy->state == XY_MODE_ON || xy->state == XY_MODE_FADEIN) {
        xy->state = XY_MODE_FADEOUT;
    }

    xy->counter = xy->len;
}

int whisper_xy_is_on(whisper_xy *xy)
{
    return xy->state == XY_MODE_ON;
}

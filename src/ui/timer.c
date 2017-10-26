#include <stdlib.h>
#include "eyejam.h"

struct jam_timer {
    double len;
    double pos;
    int mode;
};

size_t jam_timer_size()
{
    return sizeof(jam_timer);
}

void jam_timer_reset(jam_timer *t)
{
    t->mode = EYEJAM_TICKING;
    t->pos = 0.f;
}

void jam_timer_init(jam_timer *t)
{
    jam_timer_reset(t);
    jam_timer_set_length(t, DEFAULT_DWELL);
}

void jam_timer_set_length(jam_timer *t, double len)
{
    t->len = len;
}

double jam_timer_alpha(jam_timer *t)
{
    return t->pos / t->len;
}

int jam_timer_increment(jam_timer *t, double delta)
{
    int rc;
    rc = EYEJAM_TICKING;

    switch(t->mode) {
        case EYEJAM_TICKING:
            t->pos += delta;
            if(t->pos >= t->len) {
                t->pos = t->len;
                rc = EYEJAM_END;
            }
            break;
        case EYEJAM_END:
        case EYEJAM_PLEASE_REWIND: 
            rc = EYEJAM_PLEASE_REWIND;
            break;
    }

    t->mode = rc;
    return rc;
}

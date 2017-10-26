#include <stdlib.h>
#include <math.h>
#include "eyejam.h" 

struct jam_hit {
    double x;
    double y;
    double w;
    double h;
    int last_time;
    int (*test)(jam_hit *, double, double);
};

static int hit_rect(jam_hit *hit, double x, double y)
{
    int check_x;
    int check_y;
    int rc;

    check_x = x >= hit->x && x <= (hit->x + hit->w);
    
    check_y = y >= hit->y && y <= (hit->y + hit->h);

    rc = EYEJAM_MISS;
    if(check_x && check_y) {
        if(hit->last_time) {
            rc = EYEJAM_OVER;
        } else {
            hit->last_time = 1;
            rc = EYEJAM_HIT;
        }
    } else {
        if(hit->last_time) {
            rc = EYEJAM_OFF;
        } else {
            rc = EYEJAM_MISS;
        }
        hit->last_time = 0;
    }

    return rc;
}

static int hit_circ(jam_hit *hit, double x, double y)
{
    int rc;
    double dist;

    rc = EYEJAM_MISS;

    dist = sqrt((hit->x - x)*(hit->x - x) + (hit->y - y)*(hit->y -y)); 

    if(dist <= hit->w) {
        /* TODO: DRY this out (duplicate code in hit_rect) */
        if(hit->last_time) {
            rc = EYEJAM_OVER;
        } else {
            hit->last_time = 1;
            rc = EYEJAM_HIT;
        }
    } else {
        if(hit->last_time) {
            rc = EYEJAM_OFF;
        } else {
            rc = EYEJAM_MISS;
        }
        hit->last_time = 0;
    }

    return rc;
}

void jam_hit_rect(jam_hit *hit, double x, double y, double w, double h)
{
    hit->x = x;
    hit->y = y;
    hit->w = w;
    hit->h = h;
    hit->last_time = 0;
    hit->test = hit_rect;
}

void jam_hit_circ(jam_hit *hit, double x, double y, double rad)
{
    hit->x = x; /* center x */
    hit->y = y; /* center y */
    hit->w = rad; /* width is radius */
    hit->h = rad; /* height is radius */
    hit->last_time = 0;
    hit->test = hit_circ;
}

int jam_hit_test(jam_hit *hit, double x, double y)
{
    return hit->test(hit, x, y);
}

size_t jam_hit_size()
{
    return sizeof(jam_hit);
}

void jam_hit_set_pos(jam_hit *hit, double x, double y)
{
    hit->x = x;
    hit->y = y;
}

void jam_hit_set_size(jam_hit *hit, double w, double h)
{
    hit->w = w;
    hit->h = h;
}

void jam_hit_set_radius(jam_hit *hit, double rad)
{
    hit->w = rad;
}

#include <stdlib.h>
#include <math.h>
#include "ui/eyejam.h"
#include "nanovg.h"
#include "arachnoid.h"
#include "circ.h"

static arachnoid_circ circ[25];

static void circle(NVGcontext *vg, double x, double y, double rad)
{
    nvgBeginPath(vg);
    nvgCircle(vg, x, y, rad);
    nvgFill(vg);
}

void arachnoid_circ_init(
    arachnoid_circ *circ, 
    double x, 
    double y, 
    double r
)
{
    circ->x = x;
    circ->y = y;
    circ->r = r;
    circ->a = 1.0;
    circ->is_hit = 0;
    circ->g = 5.0;
}

int arachnoid_circ_test(arachnoid_circ *circ, double x, double y)
{
    double d;
    d = sqrt(
            (x - circ->x)*(x - circ->x) + 
            (y - circ->y)*(y - circ->y));
    if(d <= circ->r) {
        if(!circ->is_hit && circ->a >= 1.0) {
            circ->a = 0.05;
            circ->g = 0.1 + (((double)rand() / RAND_MAX) * 5);
            circ->is_hit = 1;
            return 2;
        }
        return 1;
    } else {
        circ->is_hit = 0;
        return 0;
    }
}

void arachnoid_circ_draw(NVGcontext *vg, arachnoid_circ *circ, double delta)
{
    if(circ->a < 1.0) {
        circ->a += circ->g * delta;
    }
    circle(vg, circ->x, circ->y, circ->r * circ->a);
}


arachnoid_circ *arachnoid_circ_get(int id)
{
    return &circ[id];
}

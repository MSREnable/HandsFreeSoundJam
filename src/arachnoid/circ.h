#ifndef CIRC_H
#define CIRC_H
struct arachnoid_circ {
    NVGcontext *vg;
    double x;
    double y;
    double r;
    double a;
    int is_hit;
    double g;
};
#endif

#include <math.h>
#include <stdlib.h>
#include "moongazing.h"
#include "ui/eyejam.h"

enum {
    MOON_AWAKEN,
    MOON_ALIVE,
    MOON_SLEEP,
    MOON_WAIT,
};

static int color_hist[NCOLORS];

struct mg_moon {
    /* radius, x, y */
    double scale;
    double r;
    double x_c;
    double y_c;
    double alpha;
    int state;
    int hit_last_time;
    int color;
    double theta;
    double speed;
    double amp;
    double off_x;
    double off_y;
    double timer;
};

static double random_number()
{
    return (double)rand() / RAND_MAX;
}

void mg_moon_relocate(mg_moon *moon)
{
    int width;
    int height;

    width = jam_win_width();
    height = jam_win_height();
  
    /* subtract radius to make sure values don't go off-screen */
    /* range should be between radius and (width - radius) */
    moon->x_c = moon->r + (width - 2.f*moon->r) * random_number();
    moon->y_c = moon->r + (height - 2.f*moon->r) * random_number();
}

static void pick_a_color(mg_moon *moon)
{
    int col;

    if(color_hist[moon->color] > 0) color_hist[moon->color]--;

    do {
        col = floor(random_number() * NCOLORS);
    } while(color_hist[col] > 1);

    moon->color = col;
    color_hist[col]++;
}

static void calculate_offset(mg_moon *moon)
{
    moon->off_x = moon->amp * cos(moon->theta * 3.0);
    moon->off_y = moon->amp * sin(moon->theta * 2.0);
}

static void change_movement(mg_moon *moon)
{
    moon->speed = 0.05 + random_number() * 0.5;
    moon->amp = 8.0 + CONSTANT(20.0) * random_number();
    moon->off_x = 0.0;
    moon->off_y = 0.0;

    /* calculate initial offset */
    calculate_offset(moon);
}

void mg_moon_init(mg_moon *moon)
{
    moon->r = CONSTANT(75.f);
    moon->scale = 1.0;
    moon->x_c = 200.f;
    moon->y_c = 200.f;
    moon->alpha = 0.f;
    moon->state = MOON_AWAKEN;
    moon->hit_last_time = 0;
    mg_moon_relocate(moon);
    moon->color = -1;
    pick_a_color(moon);

    moon->theta = 2 * M_PI * random_number();
    change_movement(moon);
}

int mg_moon_hit(mg_moon *moon, double x, double y)
{
    double d;

    double x_c;
    double y_c;

    mg_moon_pos(moon, &x_c, &y_c);
    if (moon->state != MOON_ALIVE) return 0;

    d = sqrt((x - x_c)*(x - x_c) + (y - y_c)*(y - y_c));

    if(d < moon->r) {
        if(!moon->hit_last_time) {
            moon->hit_last_time = 1;
            mg_moon_sleep(moon);
            return 1;
        }
    } else if(moon->hit_last_time) {
        moon->hit_last_time = 0;
    }
    return 0;
}

void mg_moon_step(mg_moon *moon, double delta)
{
    switch(moon->state) {
        case MOON_SLEEP:
            moon->alpha -= 1.0 * delta;
            moon->scale += 0.25 * delta;
            if(moon->alpha <= 0.0) {
                moon->alpha = 0.0;
                mg_moon_relocate(moon);
                change_movement(moon);
                pick_a_color(moon);
                moon->state = MOON_WAIT;
                moon->scale = 1.0;
                moon->timer = 5.0;
            }
            break;
        case MOON_ALIVE:
            calculate_offset(moon);
            moon->theta += moon->speed * delta;
            moon->theta = fmod(moon->theta, 2.0*M_PI);
            break;
        case MOON_AWAKEN:
            moon->alpha += 1.5 * delta;
            moon->hit_last_time = 0;
            if(moon->alpha >= 1.0) {
                moon->alpha = 1.0;
                moon->state = MOON_ALIVE;
            }
            break;
        case MOON_WAIT:
            moon->timer -= delta;
            if(moon->timer <= 0) {
                moon->state = MOON_AWAKEN;
            }
            break;
    }
}

void mg_moon_pos(mg_moon *moon, double *x, double *y)
{
    *x = moon->x_c + moon->off_x;
    *y = moon->y_c + moon->off_y;
}

void mg_moon_create(mg_moon **moon)
{
    *moon = malloc(sizeof(mg_moon));    
}

void mg_moon_destroy(mg_moon **moon)
{
    free(*moon);
}

double mg_moon_radius(mg_moon *moon)
{
    return moon->r * moon->scale;
}

double mg_moon_alpha(mg_moon *moon)
{
    return moon->alpha;
}

void mg_moon_sleep(mg_moon *moon)
{
    moon->state = MOON_SLEEP;
}

int mg_moon_color(mg_moon *moon)
{
    return moon->color;
}

int *mg_moon_pcolor(mg_moon *moon)
{
    return &moon->color;
}

void mg_moon_init_colorhist()
{
    int i;
    for(i = 0; i < NCOLORS; i++) color_hist[i] = 0;
}

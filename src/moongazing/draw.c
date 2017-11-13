#include <stdio.h>
#include "moongazing.h"
#include "nanovg.h"
#include "ui/eyejam.h"

#define CLOSE_DWELL 0.45
#define CLOSE_RADIUS 75*MOONSCALE


static double x_c = 100;
static double y_c = 100;

static unsigned char palette[][3] = {
    {82, 110, 188},
    {182, 188, 255},
    {255, 231, 157},
    {255, 157, 218},
    {157, 255, 171},
};

typedef struct {
    mg_moon *moon[NMOONS];
    mg_synth *synth;
    int count;
    int hit_last_time;
    double timer;
    double delta;
    double timer_alpha;
    int closing;
    int please_close;
    float time_elapsed;
} moongazing_data;

static moongazing_data moon_data;

static void mg_setup(moongazing_data *mg)
{
    int i;

    mg_synth_create(&mg->synth, 44100);

    mg_moon_init_colorhist();
    for(i = 0; i < NMOONS; i++) {
        mg_moon_create(&mg->moon[i]);
        mg_moon_init(mg->moon[i]);
        mg_synth_bind_color(mg->synth, i, mg_moon_pcolor(mg->moon[i]));
    }


    mg->count = 0;
    mg->hit_last_time = 0;
    mg->delta = 0;
    mg->timer = 0;
    mg->timer_alpha = 0;
    mg->closing = 0;
    mg->please_close = 0;

#ifdef USE_EYETRACKING
    mg->time_elapsed = 0.f;
#endif
}

static void mg_destroy(moongazing_data *mg)
{
    int i;

    for(i = 0; i < NMOONS; i++) {
        mg_moon_destroy(&mg->moon[i]);
    }
    mg_synth_destroy(&mg->synth);
}



static void pos_callback(double xpos, double ypos)
{
    int i;
    int hit_this_time;
    for(i = 0; i < NMOONS; i++) {
        if(mg_moon_hit(moon_data.moon[i], xpos, ypos)) {
            mg_synth_trigger(moon_data.synth, i);
            moon_data.count++;
        } 
    }
    
    /* check for bottom left corner intersection */
    hit_this_time = 0;
    if((xpos < CLOSE_RADIUS * MOONSCALE && ypos > CLOSE_RADIUS * MOONSCALE) || 
            moon_data.closing == 1) {
        moon_data.hit_last_time = 1;
        hit_this_time = 1;
        moon_data.timer += moon_data.delta;
    }

    if(hit_this_time == 0 && moon_data.hit_last_time == 1) {
        moon_data.hit_last_time = 0;
    }

    /* fade back to normal */
    if(hit_this_time == 0 && moon_data.timer > 0) {
        moon_data.timer -= 3 * moon_data.delta;
        if(moon_data.timer < 0) moon_data.timer = 0;
    }

    if(moon_data.timer > CLOSE_DWELL * (1 - moon_data.closing)) {
        moon_data.timer_alpha = (moon_data.timer - 
                (CLOSE_DWELL * (1 - moon_data.closing))) / 1.5;
    } else {
        moon_data.timer_alpha = 0;
    }

    if(moon_data.timer_alpha >= 1.0) moon_data.please_close = 1;

    if(moon_data.count >= 32) {
        printf("changing chord!\n");
        moon_data.count = 0;
        mg_synth_change_chord(moon_data.synth);
    }

}

void mg_init()
{
    mg_setup(&moon_data); 
}

void mg_clean()
{
    mg_destroy(&moon_data);
}

void mg_draw(NVGcontext *vg, double x, double y, double delta)
{
    int i;
    int win_width, win_height;
    int color;
  
    moon_data.delta = delta;
    win_width = jam_win_width();   
    win_height = jam_win_height();   

    pos_callback(x, y);
    
    for(i = 0; i < NMOONS; i++) {
        color = mg_moon_color(moon_data.moon[i]);
        mg_moon_step(moon_data.moon[i], delta);
        mg_moon_pos(moon_data.moon[i], &x_c, &y_c);
        nvgBeginPath(vg);
        nvgCircle(vg, x_c, y_c, mg_moon_radius(moon_data.moon[i]));
        nvgFillColor(vg, 
            nvgRGBA(
                palette[color][0],
                palette[color][1], 
                palette[color][2], 255 * mg_moon_alpha(moon_data.moon[i])));
        nvgFill(vg);
    }

    nvgBeginPath(vg);
    nvgCircle(vg, 0, win_height, CLOSE_RADIUS * MOONSCALE);
    nvgFillColor(vg, nvgRGBA(193, 51, 0, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, win_width, win_height);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255 * moon_data.timer_alpha));
    nvgFill(vg);
}

float mg_time_fade(void)
{
    return (float)moon_data.timer_alpha;
}

int mg_window_closing(void)
{
    return moon_data.please_close;
}

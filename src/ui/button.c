#include <stdlib.h>
#include "eyejam.h"
#include "nanovg.h"

enum {
    MODE_TRANSITION,
    MODE_CLICKED,
    SHAPE_RECT,
    SHAPE_CIRC
};

struct jam_button {
    jam_hit *hit;
    jam_timer *timer;
    double x, y;
    double w, h;
    jam_button_cb cb_hit;
    jam_button_cb cb_over;
    jam_button_cb cb_off;
    jam_button_cb trigger;
    void *ud;
    int color_mode;
    int alt_color; 
    NVGcolor color_normal;
    NVGcolor color_normal_alt[2];
    NVGcolor color_over;
    NVGcolor color_select;
    int shape;
    double scale;
    const char *text;
    char usetext;
    int id;
}; 

static void do_nothing(jam_button *but, void *ud)
{

}

size_t jam_button_size()
{
    return sizeof(jam_button);
}

void jam_button_init(jam_button *but)
{
    but->hit = malloc(jam_hit_size());
    but->timer = malloc(jam_timer_size());
    jam_hit_rect(but->hit, 10, 10, 100, 100);
    jam_timer_init(but->timer);
    but->w = 100;
    but->h = 100;
    but->x = 10;
    but->y = 10;
    but->cb_hit = do_nothing;
    but->cb_over = do_nothing;
    but->cb_off = do_nothing;
    but->trigger = do_nothing;
    but->color_mode = MODE_TRANSITION;
    but->alt_color = 0;
    but->shape = SHAPE_RECT;
    but->scale = 1.0;

    jam_button_color_normal(but, 255, 255, 255);
    jam_button_color_normal_alt(but, 0, 128, 128, 128);
    jam_button_color_normal_alt(but, 1, 0, 255, 255);
    jam_button_color_over(but, 255, 0, 0);
    jam_button_color_select(but, 0, 0, 255);
    but->usetext = 0;
}

void jam_button_free(jam_button *but)
{
    free(but->hit);
    free(but->timer);
}

void jam_button_setsize(jam_button *but, double w, double h)
{
    jam_hit_set_size(but->hit, w, h);
    but->w = w;
    but->h = h;
}

void jam_button_pos(jam_button *but, double x, double y)
{
    jam_hit_set_pos(but->hit, x, y);
    but->x = x;
    but->y = y;
}

void jam_button_reset(jam_button *but)
{
    jam_timer_reset(but->timer);
    but->color_mode = MODE_TRANSITION;
}

int jam_button_hit_test(jam_button *but, double x, double y)
{
    return jam_hit_test(but->hit, x, y);
}

int jam_button_interact(jam_button *but, double x, double y, double delta)
{
    int rc;

    rc = jam_button_hit_test(but, x, y);
    switch(rc) {
        case EYEJAM_HIT:
            jam_button_reset(but);
            but->cb_hit(but, but->ud);
            break;
        case EYEJAM_OVER:
            switch(jam_button_step(but, delta)) {
                case EYEJAM_END:
                    but->color_mode = MODE_CLICKED;
                    but->trigger(but, but->ud);
                    rc = EYEJAM_TRIGGER;
                    break;
                case EYEJAM_PLEASE_REWIND:
                    break;
                default:
                    break;
            }
            but->cb_over(but, but->ud);
            break;
        case EYEJAM_OFF:
            jam_button_reset(but);
            but->cb_off(but, but->ud);
            break;
    }
    return rc;
}

int jam_button_step(jam_button *but, double delta)
{
    return jam_timer_increment(but->timer, delta);
}

static NVGcolor crossfade(NVGcolor *c1, NVGcolor *c2, double alpha)
{
    NVGcolor out;

    out.r = (1 - alpha)*c1->r + alpha*c2->r;
    out.g = (1 - alpha)*c1->g + alpha*c2->g;
    out.b = (1 - alpha)*c1->b + alpha*c2->b;
    out.a = 1;

    return out;
}

void jam_button_draw(NVGcontext *vg, jam_button *but)
{
    double a;
    double x;
    double y;
    double w;
    double h;
    nvgBeginPath(vg);

    w = but->w * but->scale;
    h = but->h * but->scale;
   
    /* reuse a coefficient before it is used */
    a = (1 - but->scale) * 0.5;
    x = but->x + a * but->w;
    y = but->y + a * but->h;

    //nvgRect(vg, x, y, w, h);
    nvgRoundedRect(vg, x, y, w, h, CONSTANT(10));

    //if(but->color_mode == MODE_TRANSITION) {
    if(but->color_mode == MODE_TRANSITION) {
        a = jam_timer_alpha(but->timer);
        if(but->alt_color == 0) {
            nvgFillColor(vg, crossfade(&but->color_normal, &but->color_over, a)); 
        } else {
            nvgFillColor(vg, 
                    crossfade(
                        &but->color_normal_alt[but->alt_color - 1], 
                        &but->color_over, a)); 
        }
    } else {
        if(but->alt_color == 0) {
            nvgFillColor(vg, but->color_normal);
        } else {
            nvgFillColor(vg, but->color_normal_alt[but->alt_color - 1]); 
        }
    }

    nvgFill(vg);

    if(but->usetext) {
        nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_BOTTOM);
        nvgFontSize(vg, CONSTANT(28.0f));
        nvgFillColor(vg, nvgRGB(0, 0, 0));
        nvgText(vg, but->x + but->w*0.5, but->y + but->h*0.5 + CONSTANT(14), but->text, NULL);
    }

}

void jam_button_cb_hit(jam_button *but, jam_button_cb cb)
{
    but->cb_hit = cb;
}

void jam_button_cb_over(jam_button *but, jam_button_cb cb)
{
    but->cb_over = cb;
}

void jam_button_cb_off(jam_button *but, jam_button_cb cb)
{
    but->cb_off = cb;
}

void jam_button_cb_trigger(jam_button *but, jam_button_cb cb)
{
    but->trigger = cb;
}

void jam_button_color_normal(jam_button *but, int r, int g, int b)
{
    but->color_normal = nvgRGB(r, g, b);
}

void jam_button_color_normal_alt(jam_button *but, int clr, int r, int g, int b)
{
    but->color_normal_alt[clr] = nvgRGB(r, g, b);
}

void jam_button_color_over(jam_button *but, int r, int g, int b)
{
    but->color_over = nvgRGB(r, g, b);
}

void jam_button_color_select(jam_button *but, int r, int g, int b)
{
    but->color_select = nvgRGB(r, g, b);
}

void jam_button_alt_color(jam_button *but, int mode)
{
    but->alt_color = mode;
}

void jam_button_scale(jam_button *but, double scale)
{
    but->scale = scale;
}

void jam_button_text(jam_button *but, const char *text)
{
    but->text = text;
    but->usetext = 1;
}

void jam_button_data(jam_button *but, void *ud)
{
    but->ud = ud;
}

void jam_button_id(jam_button *but, int id)
{
    but->id = id;
}

int jam_button_id_get(jam_button *but)
{
    return but->id;
}

void jam_button_dwell_set(jam_button *but, double time)
{
    jam_timer_set_length(but->timer, time);
}

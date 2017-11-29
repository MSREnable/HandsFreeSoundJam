/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#ifdef NANOVG_GLEW
#include <GL/glew.h>
#endif

#ifdef IS_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <GLFW/glfw3.h>
#include <time.h>
#include "nanovg.h"
#include "nanovg_gl.h"
#include "eyejam.h"

#ifdef USE_ONEEURO
#include "SF1eFilter.h"
#endif

static int win_width, win_height;
static GLFWwindow *window;
void whisper_xy_getpos(float *x, float *y);

void jam_clear_color(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0);
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    NVG_NOTUSED(scancode);
    NVG_NOTUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void mouse(GLFWwindow *window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
        
}

int jam_win_width()
{
    return win_width;
}

int jam_win_height()
{
    return win_height;
}

void jam_close()
{
    glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
    NVGcontext *vg;
    int fbwidth, fbheight;
    float pxratio;
    double mx, my;
    double mx_n, my_n; /* normalized values */
    double t, prev_t, delta;
    float smoothX, smoothY;
    jam_ui *ui;
#ifdef USE_ONEEURO
    SF1eFilter filt_x;
    SF1eFilter filt_y;
#endif

    vg = NULL;

    if(!glfwInit()) {
        printf("Failed to init GLFW.");
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifdef DEMO_MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif
  
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
#ifdef FULLSCREEN
    window = glfwCreateWindow(mode->width, mode->height, "Moongazing", glfwGetPrimaryMonitor(), NULL);
#else
    window = glfwCreateWindow(mode->width, mode->height, "Moongazing", NULL, NULL);
#endif

    if(!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
    if(glewInit() != GLEW_OK) {
        printf("could not init GLEW\n");
        return -1;
    }
#endif

    vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

    if (vg == NULL) {
        printf("Could not init nanovg.\n");
        return -1;
    }

    /* glfwSetCursorPosCallback(window, pos_callback); */
    glfwSetKeyCallback(window, key);

    glfwSetMouseButtonCallback(window, mouse);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwGetWindowSize(window, &win_width, &win_height);

    prev_t = 0;
    t = 0;

    /* set up loading screen */
    nvgCreateFont(vg, "sans", "fonts/Roboto-Regular.ttf"); 
        
    glfwGetWindowSize(window, &win_width, &win_height);
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
    glViewport(0, 0, fbwidth, fbheight);
    pxratio = (float)fbwidth / (float)fbheight;

    nvgBeginFrame(vg, win_width, win_height, pxratio);
    nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_BOTTOM);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    nvgFontSize(vg, CONSTANT(72.0f));
    nvgFillColor(vg, nvgRGB(255, 255, 255));
    nvgText(vg, win_width / 2, win_height / 2, 
    "Loading...", NULL);
    nvgFill(vg);
    nvgEndFrame(vg);
    glfwSwapBuffers(window);

    ui = malloc(jam_ui_size());
    jam_ui_init(ui);
#ifdef USE_EYETRACKING
    jam_tobii_setup();
#endif

#ifdef USE_ONEEURO
    filt_x.config.frequency = 0.1;
    filt_x.config.cutoffSlope = 1;
    filt_x.config.derivativeCutoffFrequency = 1;
    filt_x.config.minCutoffFrequency = 1;
    SF1eFilterInit(&filt_x);

    filt_y.config.frequency = 0.1;
    filt_y.config.cutoffSlope = 1;
    filt_y.config.derivativeCutoffFrequency = 1;
    filt_y.config.minCutoffFrequency = 1;
    SF1eFilterInit(&filt_y);
#endif


    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
#ifdef USE_EYETRACKING
        jam_tobii_getxy(&mx, &my);
        /* clamp values to be between 0 and 1 */
        if(mx > 1) mx = 1;
        else if(mx < 0) mx = 0;
        if(my > 1) my = 1;
        else if(my < 0) my = 0;
#ifdef USE_ONEEURO
        mx_n = SF1eFilterDoAtTime(&filt_x, (float)mx, t);
        my_n = SF1eFilterDoAtTime(&filt_y, (float)my, t);
        mx = mx_n * win_width;
        my = my_n * win_height;
#else
        mx_n = mx;
        my_n = my;
        mx *= win_width;
        my *= win_height;
#endif

#else
        glfwGetCursorPos(window, &mx, &my);
        mx_n = mx / win_width;
        my_n = my / win_height;
#endif
        glfwGetWindowSize(window, &win_width, &win_height);
        glfwGetFramebufferSize(window, &fbwidth, &fbheight);

        glViewport(0, 0, fbwidth, fbheight);
        pxratio = (float)fbwidth / (float)fbheight;
        nvgBeginFrame(vg, win_width, win_height, pxratio);

        t = glfwGetTime();
        delta = t - prev_t;
      
        jam_xy_pos_set(mx_n, my_n);
        jam_ui_step(vg, ui, mx, my, delta);

/* uncomment to draw centering lines */
//#define DRAW_GRID
#ifdef DRAW_GRID
        nvgBeginPath(vg);
        nvgStrokeWidth(vg, 1.5);
        nvgMoveTo(vg, win_width / 2, 0);
        nvgLineTo(vg, win_width / 2, win_height);
        nvgStrokeColor(vg, nvgRGB(255, 255, 255));
        nvgStroke(vg);
        
        nvgBeginPath(vg);
        nvgMoveTo(vg, 0, win_height/2);
        nvgLineTo(vg, win_width, win_height / 2);
        nvgStrokeColor(vg, nvgRGB(255, 255, 255));
        nvgStroke(vg);
/* 1024 x 768 guides */
        int offset = (win_width - CONSTANT(1024)) / 2;
        nvgBeginPath(vg);
        nvgMoveTo(vg, CONSTANT(1024) + offset, 0);
        nvgLineTo(vg, CONSTANT(1024) + offset, win_height);
        nvgStrokeColor(vg, nvgRGB(255, 255, 255));
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgMoveTo(vg, offset, 0);
        nvgLineTo(vg, offset, win_height);
        nvgStrokeColor(vg, nvgRGB(255, 255, 255));
        nvgStroke(vg);

        offset = (win_height - CONSTANT(768)) / 2;
        nvgBeginPath(vg);
        nvgMoveTo(vg, 0, CONSTANT(768) + offset);
        nvgLineTo(vg, win_width, CONSTANT(768) + offset);
        nvgStrokeColor(vg, nvgRGB(255, 255, 255));
        nvgStroke(vg);
        nvgBeginPath(vg);
        nvgMoveTo(vg, 0, offset);
        nvgLineTo(vg, win_width, offset);
        nvgStrokeColor(vg, nvgRGB(255, 255, 255));
        nvgStroke(vg);

#endif

        /* draw crosshairs */
        if(jam_xy_is_on()) {
            whisper_xy_getpos(&smoothX, &smoothY);
            smoothX *= win_width;
            smoothY = (1 - smoothY);
            smoothY *= win_height;
            nvgBeginPath(vg);
            nvgMoveTo(vg, 0, smoothY);
            nvgLineTo(vg, win_width, smoothY); 
            nvgStrokeColor(vg, nvgRGB(255, 255, 255));
            nvgStroke(vg);
            
            nvgBeginPath(vg);
            nvgMoveTo(vg, smoothX, 0);
            nvgLineTo(vg, smoothX, win_height); 
            nvgStrokeColor(vg, nvgRGB(255, 255, 255));
            nvgStroke(vg);
        }

        /* draw pointer */
#ifdef DRAW_POINTER
        nvgBeginPath(vg);
        nvgCircle(vg, mx, my, CONSTANT(5.f));
        nvgFillColor(vg, nvgRGB(255, 0, 0));
        nvgFill(vg);
#endif


        nvgTextAlign(vg, NVG_ALIGN_RIGHT|NVG_ALIGN_BOTTOM);
    
        nvgFontSize(vg, CONSTANT(14.0f));
        nvgFillColor(vg, nvgRGB(255, 255, 255));
        nvgText(vg, win_width, win_height, 
        "Microsoft Hands-Free Sound Jam", NULL);

        nvgEndFrame(vg);
        glfwSwapBuffers(window);
        glfwPollEvents();

        prev_t = t;
    }

#ifdef USE_EYETRACKING
    jam_tobii_destroy();
#endif
    jam_ui_free(ui);
    free(ui);
    nvgDeleteGL2(vg);
    glfwTerminate();

    return 0;
}

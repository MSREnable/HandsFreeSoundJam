/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#ifndef ARACHNOID_CIRC_H
#define ARACHNOID_CIRC_H

#ifndef JAM_UI_DATA
#define JAM_UI_DATA
typedef struct jam_ui jam_ui;
#endif

typedef struct NVGcontext NVGcontext;
typedef struct arachnoid_circ arachnoid_circ;
void arachnoid_circ_init(
    arachnoid_circ *circ, 
    double x, 
    double y, 
    double r
);
int arachnoid_circ_test(arachnoid_circ *circ, double x, double y);
void arachnoid_circ_draw(NVGcontext *vg, arachnoid_circ *circ, double delta);
arachnoid_circ *arachnoid_circ_get(int id);

void arachnoid_audio_init();
void arachnoid_audio_clean();
void arachnoid_init(jam_ui *ui);
void arachnoid_step(NVGcontext *vg, double x, double y, double delta);
void arachnoid_clean();
void arachnoid_please_tick(int id);
float arachnoid_tick();
void arachnoid_audio_stop();

#endif

/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "dsp/dsp.h"
#include "eyejam.h"
#include "nanovg.h"
size_t jam_config_size();
void jam_config_init(jam_config *config, jam_ui *ui);
void jam_config_free(jam_config *config);
void jam_config_interact(jam_config *config, double x, double y, double step);
void jam_config_draw(NVGcontext *vg, jam_config *config);
void jam_config_step(NVGcontext *vg, jam_config *config, double x, double y, double step);

#ifdef USE_EYETRACKING
#include <tobii/tobii_streams.h>
#include <stdio.h>
#include <assert.h>
#include "eyejam.h"

static tobii_api_t *api;
static tobii_error_t tobii_error;
static tobii_device_t *device;
static double tobii_pos_x = 0;
static double tobii_pos_y = 0;

static void gaze_pointer_callback(tobii_gaze_point_t const* gaze_point, 
    void *ud)
{
    if(gaze_point->validity == TOBII_VALIDITY_VALID) {
        /*
        printf("Gaze point: %f, %f\n",
            gaze_point->position_xy[0],
            gaze_point->position_xy[1]);
        */

        tobii_pos_x = gaze_point->position_xy[0];
        tobii_pos_y = gaze_point->position_xy[1];
    }

}

void jam_tobii_setup()
{
    tobii_error = tobii_api_create(&api, NULL, NULL);
	assert(tobii_error == TOBII_ERROR_NO_ERROR);


    tobii_error = tobii_device_create(api, NULL, &device);
	assert(tobii_error == TOBII_ERROR_NO_ERROR);


    tobii_error = tobii_gaze_point_subscribe(device, 
        gaze_pointer_callback, 
        0);

	assert(tobii_error == TOBII_ERROR_NO_ERROR);

}

void jam_tobii_destroy()
{
	tobii_error = tobii_api_destroy(api);

	assert(tobii_error == TOBII_ERROR_NO_ERROR);

}

void jam_tobii_getxy(double *x, double *y)
{
    tobii_error = tobii_wait_for_callbacks(device);
    assert(tobii_error == TOBII_ERROR_NO_ERROR ||
        tobii_error == TOBII_ERROR_TIMED_OUT);
    
    tobii_error = tobii_process_callbacks(device);
    assert(tobii_error == TOBII_ERROR_NO_ERROR);

    *x = tobii_pos_x;
    *y = tobii_pos_y;
}
#endif

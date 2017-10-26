#include "dsp.h"

static sp_data *global_sp = NULL;
static int is_created = 0;

void whisper_sp_create(int sr)
{
    if(is_created == 0) {
        is_created = 1;
        sp_create(&global_sp);
        global_sp->sr = sr;
    }
}

int whisper_sp_is_created()
{
    return is_created;
}

void whisper_sp_destroy()
{
    if(is_created && global_sp != NULL) {
        sp_destroy(&global_sp);
    }
}    

sp_data * whisper_sp_data()
{
    return global_sp;
}

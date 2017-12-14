#include "dsp.h"

static void process(sp_data *sp, void *ud)
{
    SPFLOAT tmp;
    whisper_eyejam_tick(&tmp);
    sp_out(sp, 0, tmp * 0.5);
}

int main(int argc, char *argv[])
{
    sp_data *sp;
    whisper_eyejam_init(44100);
    sp = whisper_sp_data();
    sp->len = sp->sr * 4;
/*
    whisper_eyejam_db_open("data.db");
    whisper_eyejam_db_load_default();
*/
    whisper_schedule(1, 0);
    whisper_eyejam_demo_clips();

    whisper_trinity_dump(0);
    sp_process(sp, NULL, process); 
/*
    whisper_eyejam_db_save_default();
    whisper_eyejam_db_close();
*/
    whisper_eyejam_destroy();
    return 0;
}

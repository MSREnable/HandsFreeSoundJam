#include <stdlib.h>
#include <soundpipe.h>
#include "loadflac.h"

typedef struct {
    sp_ftbl *ft;
    unsigned long counter;
} user_data;

static void process(sp_data *sp, void *ud)
{
    SPFLOAT out;
    user_data *data;

    data = ud;
    out = data->ft->tbl[data->counter];
    data->counter++;
    data->counter %= data->ft->size;
    sp_out(sp, 0, out);
}

int main()
{
    sp_data *sp;
    user_data ud;

    sp_create(&sp);

    ud.counter = 0;
    sp_ftbl_loadflac(sp, &ud.ft, "oneart.flac");

    sp_process(sp, &ud, process);
    sp_ftbl_destroy(&ud.ft);
    sp_destroy(&sp);
    return 0;
}

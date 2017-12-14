#define _BSD_SOURCE
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#include <math.h>
#include <soundpipe.h>

#include "loadflac.h"

int sp_ftbl_loadflac(sp_data *sp, sp_ftbl *ft, const char *filename)
{
    size_t size;
    unsigned int channels;
    unsigned int samplerate;
/*
    SNDFILE *snd = sf_open(filename, SFM_READ, &info);
    if(snd == NULL) {
        return SP_NOT_OK;
    }
*/

#ifdef USE_DOUBLE
    fprintf(stderr, "Doubles not supported yet.\n");
    return SP_NOT_OK;
#else
    ft->tbl = drflac_open_and_decode_file_f32(filename, 
            &channels,
            &samplerate,
            &size);
#endif

    if(ft->tbl == NULL) {
        ft->del = 0;
        fprintf(stderr, "Could not open %s\n", filename);
        return SP_NOT_OK;
    }

    sp_ftbl_init(sp, ft, size);
/*
    ft->size = size;
    ft->sicvt = 1.0f * SP_FT_MAXLEN / sp->sr;
    ft->lobits = (uint32_t)log2(SP_FT_MAXLEN / (double)size);
    ft->lomask = (1<<ftp->lobits) - 1;
    ft->lodiv = 1.0f / (1 << ftp->lobits);
    ft->del = 1;
*/

    return SP_OK;
}

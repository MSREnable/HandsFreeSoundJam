#define _BSD_SOURCE
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#include <math.h>
#include <soundpipe.h>

#include "loadflac.h"

int sp_ftbl_loadflac(sp_data *sp, sp_ftbl **ft, const char *filename)
{
    size_t size;
    unsigned int channels;
    unsigned int samplerate;
    sp_ftbl *ftp;
    *ft = malloc(sizeof(sp_ftbl));
    ftp = *ft;
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
    ftp->tbl = drflac_open_and_decode_file_f32(filename, 
            &channels,
            &samplerate,
            &size);
#endif

    if(ftp->tbl == NULL) {
        ftp->del = 0;
        fprintf(stderr, "Could not open %s\n", filename);
        return SP_NOT_OK;
    }

    ftp->size = size;
    ftp->sicvt = 1.0f * SP_FT_MAXLEN / sp->sr;
    ftp->lobits = (uint32_t)log2(SP_FT_MAXLEN / (double)size);
    ftp->lomask = (1<<ftp->lobits) - 1;
    ftp->lodiv = 1.0f / (1 << ftp->lobits);
    ftp->del = 1;

    return SP_OK;
}

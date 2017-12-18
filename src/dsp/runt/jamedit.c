#include <stdio.h>
#include <runt.h>
#include "dsp.h"


#define PROC(name) rproc_##name
#define FUN(name) static int PROC(name)(runt_vm *vm, runt_ptr p)
#define KEYWORD(NAME,SIZE,FUN) runt_keyword_define(vm,NAME,SIZE,FUN,NULL)


FUN(foo)
{
    runt_print(vm, "we are foo.\n");
    return RUNT_OK;
}

FUN(insert)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int nn;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    nn = s->f;

    whisper_eyejam_edit_insert(nn);

    return RUNT_OK;
}

FUN(save_song)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int id;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    id = s->f;

    whisper_eyejam_db_save_song(id);
    return RUNT_OK;
}

FUN(load_song)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int id;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    id = s->f;

    whisper_eyejam_db_load_song(id);
    return RUNT_OK;
}

FUN(clip_set_length)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int len;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    len = s->f;
    whisper_eyejam_edit_set_clip_length(len);
    return RUNT_OK;
}

FUN(loopmode)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int mode;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    mode = s->f;

    whisper_tracks_loopmode(whisper_eyejam_edit_get_track(), mode);

    return RUNT_OK;
}

FUN(set_notelen)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int len;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    len = s->f;

    whisper_eyejam_edit_set_notelen(len);

    return RUNT_OK;
}

FUN(set_track)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int track;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    track = s->f;

    whisper_eyejam_edit_set_track(track);

    return RUNT_OK;
}

FUN(set_clip)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int clip;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    clip = s->f;

    whisper_eyejam_edit_set_clip(clip);

    return RUNT_OK;
}

FUN(set_base)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int base;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    base = s->f;

    whisper_eyejam_edit_set_base(base);

    return RUNT_OK;
}

FUN(set_pos)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int pos;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    pos = s->f;

    whisper_eyejam_edit_set_pos(pos);

    return RUNT_OK;
}

FUN(voice_up)
{
    whisper_eyejam_edit_voice_up();
    return RUNT_OK;
}

FUN(voice_down)
{
    whisper_eyejam_edit_voice_down();
    return RUNT_OK;
}

FUN(stepsize_double)
{
    whisper_eyejam_edit_stepsize_double();
    return RUNT_OK;
}

FUN(stepsize_half)
{
    whisper_eyejam_edit_stepsize_half();
    return RUNT_OK;
}

FUN(trinity_preset)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int instr;
    runt_int preset;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    instr = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    preset = s->f;

    whisper_trinity_preset(instr, preset);

    return RUNT_OK;
}

FUN(surgeon_preset)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int instr;
    runt_int preset;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    instr = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    preset = s->f;

    whisper_surgeon_preset(instr, preset);

    return RUNT_OK;
}

FUN(set_title)
{
    runt_int rc;
    runt_stacklet *s;
    const char *title;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    title = runt_to_string(s->p);

    whisper_eyejam_title_set(title);

    return RUNT_OK;
}

FUN(set_tempo)
{
    runt_int rc;
    runt_stacklet *s;
    runt_float tempo;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    tempo = s->f;

    whisper_eyejam_tempo_set(tempo);

    return RUNT_OK;
}

static runt_int loader(runt_vm *vm)
{
    runt_load_stdlib(vm);
    KEYWORD("foo", 3, PROC(foo));
    KEYWORD("insert", 6, PROC(insert));
    KEYWORD("save_song", 9, PROC(save_song));
    KEYWORD("load_song", 9, PROC(load_song));
    KEYWORD("set_clip_length", 15, PROC(clip_set_length));
    KEYWORD("loopmode", 8, PROC(loopmode));
    KEYWORD("set_notelen", 11, PROC(set_notelen));
    KEYWORD("set_track", 9, PROC(set_track));
    KEYWORD("set_clip", 8, PROC(set_clip));
    KEYWORD("set_pos", 7, PROC(set_pos));
    KEYWORD("voice_up", 8, PROC(voice_up));
    KEYWORD("voice_down", 8, PROC(voice_down));
    KEYWORD("stepsize_double", 15, PROC(stepsize_double));
    KEYWORD("stepsize_half", 13, PROC(stepsize_half));
    KEYWORD("set_base", 8, PROC(set_base));
    KEYWORD("trinity_preset", 14, PROC(trinity_preset));
    KEYWORD("surgeon_preset", 14, PROC(surgeon_preset));
    KEYWORD("set_title", 9, PROC(set_title));
    KEYWORD("set_tempo", 9, PROC(set_tempo));
    return runt_is_alive(vm);
}

int main(int argc, char *argv[])
{
    int rc;
    whisper_eyejam_init(44100);
    whisper_eyejam_db_open("data.db");
    rc = irunt_begin(argc, argv, loader);
    whisper_eyejam_db_close();
    whisper_eyejam_destroy();
    return rc;
}

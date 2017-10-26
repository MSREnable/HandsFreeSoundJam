#include <math.h>
#include "dsp.h"
#define DB2LIN(A) pow(10, (double)(A)/20)
#define LIN2DB(A) (20*log10((double)A))

#include "demo_clips.h"

#define LENGTH(x) ((int)(sizeof(x) / sizeof *(x)))

typedef struct whisper_eyejam whisper_eyejam;

struct whisper_eyejam
{
    sp_data *sp;
    sp_clock *clk;

    /* click track */
    sp_ftbl *ft;
    sp_osc *osc;
    sp_tenvx *env;
    int ticker;
    int counter;
    int measure;
    int metro;
    SPFLOAT gain;

    /* tempo-synced "pulse" used by UI elements */
    sp_tenv *pulse;
    SPFLOAT s_pulse;

    /* tempo of project, in beats per minute */
    SPFLOAT tempo;

    /* track data */
    whisper_track *tracks;

    /* XY location */
    whisper_xy *xy;

    /* clip limiter */
    sp_clip *clipL;
    sp_clip *clipR;
    int please_play;
    int please_stop;
    int am_i_playing;
};

static whisper_eyejam eyejam;


static void eyejam_setup(whisper_eyejam *ej)
{
    ej->tempo = 120.f;

    ej->ticker = 0;
    ej->counter = 0;
    ej->measure = 0;
    ej->metro = 0;
    ej->gain = 1.f;
    ej->s_pulse = 0.f;
    ej->please_play = 0;
    ej->please_stop = 0;
    ej->am_i_playing = 1;
}

static void eyejam_init(whisper_eyejam *ej, int sr)
{
    /* TODO: remove local sp instances */
    sp_create(&ej->sp);
    
    /* instantiate global soundpipe data struct */

    ej->sp->sr = sr;

    sp_clock_create(&ej->clk);
    sp_clock_init(ej->sp, ej->clk);

    ej->clk->bpm = ej->tempo;

    /* subdivide the beat into 16th notes and smaller subdivisions for ticks */
    ej->clk->subdiv = 4.f * WHISPER_NTICKS;

    /* set up click track */
    sp_ftbl_create(ej->sp, &ej->ft, 8192);
    sp_gen_sine(ej->sp, ej->ft);
    sp_osc_create(&ej->osc);
    sp_osc_init(ej->sp, ej->osc, ej->ft, 0.f);

    sp_tenvx_create(&ej->env);
    sp_tenvx_init(ej->sp, ej->env);
    ej->env->atk = 0.001f;
    ej->env->hold = 0.001f;
    ej->env->rel = 0.01f;

    sp_tenv_create(&ej->pulse);
    sp_tenv_init(ej->sp, ej->pulse);
    ej->pulse->atk = 0.01f;
    ej->pulse->hold = 0.001f;
    ej->pulse->rel = 0.5f;

    ej->tracks = whisper_tracks_global_data();

    ej->xy = whisper_xy_global_data();

    whisper_xy_init(ej->xy, ej->sp);

    sp_clip_create(&ej->clipL);
    sp_clip_init(ej->sp, ej->clipL);
    ej->clipL->lim = 1.f;
    
    sp_clip_create(&ej->clipR);
    sp_clip_init(ej->sp, ej->clipR);
    ej->clipR->lim = 1.f;
}

void whisper_eyejam_init(int sr)
{
    eyejam_setup(&eyejam);
    whisper_sp_create(sr);
    whisper_tracks_init(sr);
    /* set up the mixer to zero out stuff */
    /* effect callbacks get set here */
    whisper_mixer_setup();

    /* init will call the init callbacks for effects */
    whisper_mixer_init(sr);

    /* Trinity setup */
    /* set track 0 to hold the master trinity track 
     * This is mandatory, as this track allocates all trinity voices  
     * */
    whisper_trinity_set_master(0);

    /* set up voice groups for trinity */

    /* assign preset 1 (bass) to voice group 1 */
    whisper_trinity_vgroup_preset(1, 1);

    /* bind instruments to tracks */
    
    whisper_trinity_bind_track(0, 0);
    whisper_drumkit_bind_track(1);

    /* this is a subordinate instance of Trinity, playing a different preset */
    whisper_trinity_bind_track(1, 2);

    /* Surgeon */
    whisper_surgeon_setup();
    whisper_surgeon_bind_track(0, 3);

    /* Surgeon 2 for alternate melody track */
    whisper_surgeon_bind_track(1, 4);

    /* initialize all instruments */
    whisper_tracks_init_instruments(sr);

    /* set trinity voice state to be ON */
    whisper_trinity_voice_state(0, 0, 1);
    
    /* set bass voice state to be ON */
    whisper_trinity_voice_state(1, 0, 1);
    eyejam_init(&eyejam, sr);

    /* set up some reasonable gains */
    
    whisper_tracks_gain(0, -8.f);
    whisper_tracks_gain(1, -4.f);
    whisper_tracks_gain(2, -1.f);
    whisper_tracks_gain(3, 2.f);
    whisper_tracks_gain(4, -5.f);

    /* set up looping for tracks */
    whisper_tracks_loopmode(1, 1);
    whisper_tracks_loopmode(2, 1);
    whisper_tracks_loopmode(3, 1);
    whisper_tracks_loopmode(4, 1);
    
    whisper_eyejam_gain(0.f);

    /* set up eyejam editor interface */
    whisper_eyejam_edit_init();

    /* set eyejam tempo */
    whisper_eyejam_tempo_set(120);
}

static void eyejam_destroy(whisper_eyejam *ej)
{
    sp_clock_destroy(&ej->clk);
    sp_ftbl_destroy(&ej->ft);
    sp_osc_destroy(&ej->osc);
    sp_tenvx_destroy(&ej->env);
    sp_tenv_destroy(&ej->pulse);
    /*TODO: remove local sp data instances */
    whisper_xy_cleanup(ej->xy);
    sp_clip_destroy(&ej->clipL);
    sp_clip_destroy(&ej->clipR);
    sp_destroy(&ej->sp);
    whisper_sp_destroy();
}

void whisper_eyejam_destroy(void)
{
    whisper_tracks_destroy();
    whisper_mixer_destroy();
    eyejam_destroy(&eyejam);
}

void whisper_eyejam_tick(SPFLOAT *sample)
{
    SPFLOAT tmp;
    whisper_eyejam_tick_stereo(sample, &tmp);
}

void whisper_eyejam_tick_stereo(SPFLOAT *sampleL, SPFLOAT *sampleR)
{
    SPFLOAT s_clk;
    SPFLOAT s_beat;
    SPFLOAT s_env;
    SPFLOAT s_osc;
    SPFLOAT clock_tick;
    SPFLOAT s_click;
    SPFLOAT xyL;
    SPFLOAT xyR;

    *sampleL = 0;
    *sampleR = 0;
    clock_tick = 0.f;

    /* sort out any play/stop buttons, then return if not playing */
    if(eyejam.please_play) {
        eyejam.please_play = 0;
        eyejam.am_i_playing = 1;
        clock_tick = 1.f;
    }

    /* stop should go last because silence is safer on the ears */
    if(eyejam.please_stop) {
        eyejam.please_stop = 0;
        eyejam.am_i_playing = 0;
    }

    if(eyejam.am_i_playing == 0) return;

    s_clk = 0.f;
    s_beat = 0.f; 
    s_click = 0.f;

    /* compute clock */
    eyejam.clk->bpm = eyejam.tempo;
    sp_clock_compute(eyejam.sp, eyejam.clk, &clock_tick, &s_clk);
    /* printf("counter is %d\n", eyejam.clk->counter); */

    /* compute click track */
    if(s_clk != 0) {
        if(eyejam.ticker == 0) {
            s_beat = s_clk;
            s_beat *= (eyejam.counter == 0);

            if(eyejam.counter == 0) {
                if(eyejam.measure == 0) {
                    eyejam.osc->freq = 1000;
                } else {
                    eyejam.osc->freq = 500;
                }
                eyejam.measure++;
                eyejam.measure %= 4;
            }

            eyejam.counter++;
            eyejam.counter %= 4;
        }
        eyejam.ticker++;
        eyejam.ticker %= WHISPER_NTICKS;
    } 

    if(eyejam.metro) {
        sp_tenvx_compute(eyejam.sp, eyejam.env, &s_beat, &s_env);
        sp_osc_compute(eyejam.sp, eyejam.osc, NULL, &s_osc);

        s_click = (s_osc * s_env);
    }

    /* compute UI pulse signal */
    sp_tenv_compute(eyejam.sp, eyejam.pulse, &s_beat, &eyejam.s_pulse);

    /* compute tracks and effects inside mixer */
    whisper_mixer_tick(&s_clk, sampleL, sampleR);

    *sampleL = ((*sampleL) + s_click) * eyejam.gain;
    *sampleR = ((*sampleR) + s_click) * eyejam.gain;

    whisper_xy_compute(eyejam.xy, sampleL, sampleR, &xyL, &xyR);

    *sampleL = xyL;
    *sampleR = xyR;
/*
    sp_clip_compute(eyejam.sp, eyejam.clipL, &xyL, &s_clipL);
    sp_clip_compute(eyejam.sp, eyejam.clipR, &xyR, &s_clipR);

    *sampleL = s_clipL;
    *sampleR = s_clipR;
*/
}

void whisper_eyejam_compute(SPFLOAT *output, unsigned int nframes)
{
    unsigned int i;
    for (i = 0; i < nframes * 2; i += 2) {
        whisper_eyejam_tick_stereo(&output[i], &output[i + 1]);
    }
}

static void eyejam_populate_clip(int track, int clip, 
        const unsigned char *data, unsigned int len)
{
    unsigned int i;

    for(i = 0; i < len; i+=3) {
        whisper_clip_set_note(track, clip, data[i], data[i+1], data[i+2]); 
    }
}

void eyejam_populate_clip_with_args(int track, int clip, 
        const unsigned char *data, unsigned int len, unsigned int nargs)
{
    unsigned int i;
    unsigned int a;

    unsigned int argsize;

    argsize = 3 + nargs;

    for(i = 0; i < len; i+=argsize) {
        whisper_clip_set_note(track, clip, data[i], data[i+1], data[i+2]); 
        for(a = 0; a < nargs; a++) {
            whisper_clip_set_arg(track, clip, data[i], data[i+1], a, data[i+3+a]); 
        }
    }
}

static void eyejam_populate_clip_with_gates(int track, int clip, 
        const unsigned char *data, unsigned int len)
{
    /*
    unsigned int i;

    for(i = 0; i < len; i+=4) {
        whisper_clip_set_note(track, clip, data[i], data[i+1], data[i+2]); 
        whisper_clip_set_gate(track, clip, data[i], data[i+1], data[i+3]); 
    }
    */
    eyejam_populate_clip_with_args(track, clip, data, len, 1);
}

EXPORT void whisper_eyejam_demo_clips(void)
{
    /* set up melody clips */
    whisper_clip_set_length(0, 0, 32);
    eyejam_populate_clip(0, 0, melody_1, LENGTH(melody_1));

    whisper_clip_set_length(0, 1, 32);
    eyejam_populate_clip(0, 1, melody_2, LENGTH(melody_2));
    
    whisper_clip_set_length(0, 2, 32);
    eyejam_populate_clip(0, 2, melody_3, LENGTH(melody_3));
    
    whisper_clip_set_length(0, 3, 32);
    eyejam_populate_clip_with_gates(0, 3, melody_4, LENGTH(melody_4));
    
    whisper_clip_set_length(0, 4, 32);
    eyejam_populate_clip(0, 4, melody_5, LENGTH(melody_5));
   
    /* set up drum loops */
    whisper_clip_set_length(1, 0, 16);
    eyejam_populate_clip(1, 0, drum_loop_1, LENGTH(drum_loop_1));

    whisper_clip_set_length(1, 1, 16);
    eyejam_populate_clip(1, 1, drum_loop_2, LENGTH(drum_loop_2));
    
    whisper_clip_set_length(1, 2, 16);
    eyejam_populate_clip(1, 2, drum_loop_3, LENGTH(drum_loop_3));
    
    whisper_clip_set_length(1, 3, 16);
    eyejam_populate_clip(1, 3, drum_loop_4, LENGTH(drum_loop_4));
    
    whisper_clip_set_length(1, 4, 16);
    eyejam_populate_clip(1, 4, drum_loop_5, LENGTH(drum_loop_5));
   
    /* set up basslines */
    whisper_clip_set_length(2, 0, 32);
    eyejam_populate_clip_with_args(2, 0, bass_loop_1, LENGTH(bass_loop_1), 2);
    
    whisper_clip_set_length(2, 1, 32);
    eyejam_populate_clip_with_args(2, 1, bass_loop_2, LENGTH(bass_loop_2), 2);
    
    whisper_clip_set_length(2, 2, 32);
    eyejam_populate_clip_with_args(2, 2, bass_loop_3, LENGTH(bass_loop_3), 2);
    
    whisper_clip_set_length(2, 3, 32);
    eyejam_populate_clip_with_args(2, 3, bass_loop_4, LENGTH(bass_loop_4), 2);
    
    whisper_clip_set_length(2, 4, 32);
    eyejam_populate_clip_with_args(2, 4, bass_loop_5, LENGTH(bass_loop_5), 2);
   
    /* set up chord progressions */
    whisper_clip_set_length(3, 0, 32);
    eyejam_populate_clip(3, 0, chord_loop_1, LENGTH(chord_loop_1));
    
    whisper_clip_set_length(3, 1, 32);
    eyejam_populate_clip(3, 1, chord_loop_2, LENGTH(chord_loop_2));
    
    whisper_clip_set_length(3, 2, 32);
    eyejam_populate_clip(3, 2, chord_loop_3, LENGTH(chord_loop_3));
    
    whisper_clip_set_length(3, 3, 32);
    eyejam_populate_clip(3, 3, chord_loop_4, LENGTH(chord_loop_4));
    
    whisper_clip_set_length(3, 4, 32);
    eyejam_populate_clip(3, 4, chord_loop_5, LENGTH(chord_loop_5));
    
    /* set up alternate melodies */
    whisper_clip_set_length(4, 0, 32);
    eyejam_populate_clip(4, 0, altmelody_1, LENGTH(altmelody_1));
    whisper_clip_set_length(4, 1, 32);
    eyejam_populate_clip(4, 1, altmelody_2, LENGTH(altmelody_2));
    whisper_clip_set_length(4, 2, 32);
    eyejam_populate_clip(4, 2, altmelody_3, LENGTH(altmelody_3));
    whisper_clip_set_length(4, 3, 32);
    eyejam_populate_clip(4, 3, altmelody_4, LENGTH(altmelody_4));
    whisper_clip_set_length(4, 4, 32);
    eyejam_populate_clip(4, 4, altmelody_5, LENGTH(altmelody_5));
}

EXPORT void whisper_eyejam_metro(int mode)
{
    eyejam.metro = mode;
}

EXPORT void whisper_eyejam_gain(SPFLOAT gain)
{
    eyejam.gain = DB2LIN(gain);
}

EXPORT float whisper_eyejam_gain_get()
{
    return LIN2DB(eyejam.gain);
}

/* play the maximum number of clips on loop. make sure things don't glitch */
EXPORT void whisper_eyejam_stress_test()
{
    whisper_eyejam_demo_clips();
    whisper_tracks_loopmode(0, 1);
    whisper_tracks_loopmode(1, 1);
    whisper_tracks_loopmode(2, 1);
    whisper_tracks_loopmode(3, 1);

    whisper_schedule(0, 0);
    whisper_schedule(1, 0);
    whisper_schedule(2, 0);
    whisper_schedule(3, 0);

}

EXPORT float whisper_eyejam_pulse()
{
    return eyejam.s_pulse;
}

EXPORT float whisper_eyejam_tempo_get()
{
    return eyejam.tempo;
}

EXPORT void whisper_eyejam_tempo_set(float tempo)
{
    eyejam.tempo = tempo;
}

whisper_eyejam * whisper_eyejam_global_data()
{
    return &eyejam;
}

EXPORT void whisper_eyejam_play()
{
    eyejam.please_play = 1;
}

EXPORT void whisper_eyejam_stop()
{
    eyejam.please_stop = 1;
}

EXPORT int whisper_eyejam_beat()
{
    return eyejam.counter;
}

EXPORT void whisper_eyejam_xy_state_set(int state)
{
    whisper_xy_state(eyejam.xy, state);
}

EXPORT void whisper_eyejam_xy_pos_set(SPFLOAT x, SPFLOAT y)
{
    whisper_xy_setpos(eyejam.xy, x, y);
}

EXPORT int whisper_eyejam_xy_state_get()
{
    return whisper_xy_state_get(eyejam.xy);
}

EXPORT void whisper_eyejam_xy_toggle()
{
    whisper_xy_state_toggle(eyejam.xy);
}

EXPORT int whisper_eyejam_xy_is_on()
{
    return whisper_xy_is_on(eyejam.xy);
}

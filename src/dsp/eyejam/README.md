# Eyejam

Eyejam is the name of the software driving the creation of the whisper library.
It is a gaze-controlled clip launcher based on the clip launching mechanics
of the music software Ableton Live.


## Exported functions

### whisper\_eyejam\_demo\_clips

    EXPORT void whisper_eyejam_demo_clips(void);

### whisper\_eyejam\_metro

    EXPORT void whisper_eyejam_metro(int mode);

### whisper_eyejam_gain

    EXPORT void whisper_eyejam_gain(SPFLOAT gain);

### whisper\_eyejam\_stress\_test

    EXPORT void whisper_eyejam_stress_test();

### whisper\_eyejam\_pulse

    EXPORT float whisper_eyejam_pulse();

### whisper\_eyejam\_tempo\_get

    EXPORT float whisper_eyejam_tempo_get();

### whisper\_eyejam\_tempo\_set

    EXPORT float whisper_eyejam_tempo_set(float tempo);

### whisper\_eyejam\_play

    EXPORT void whisper_eyejam_play();

Start playing eyejam. Eyejam is set to play by default.

### whisper\_eyejam\_stop

    EXPORT void whisper_eyejam_stop();

### whisper\_eyejam\_beat

    EXPORT int whisper_eyejam_beat();

This returns the beat's current position in the bar. This is a value from 0 to 3.

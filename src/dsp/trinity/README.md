NOTE: trinity is being slightly reworked from the inside to better handle
multiple instances of itself. The API definitions here may be slightly out of
date. Refer to the header file [dsp.h](../dsp.h) for more info.

# Trinity

A 3-oscillator subtractive polyphonic synthesizer. Typically, this synthesizer 
would be spawned inside of EyeJam. However, it can also be spawned as a 
standalone program. The API below shows the exported functions you can use to 
control the synthesizer. 

## Basic Voice Control and Setup

The following functions describe how to set up Trinity and do basic voice
control to get Trinity to make sound. A *voice* in Trinity is a single sound.
Trinity has a maximum of 5 voices that can be played at once. This can be
changed with the ``TRINITY_NVOICES`` macro found at the top of 
[trinity.c](trinity.c#L2). Each voice has an id mapped 0 to ``TRINITY_NVOICES``-1,
0 to 4 by default.
For functions that require individual note control, such as setting the pitch
of a note or turning a note on or off, knowledge of the voice ID is required. 

Voices are grouped together to form a single instrument. Instruments allow
for multiple instances of Trinity to be played at once. Each instrument can 
be programmed to have their own sound. For instance, the default EyeJam demo
uses two separate Trinity intruments for the bass and melody instruments.


### whisper\_set\_trinity

    EXPORT void whisper_set_trinity(void);

This should be called before audio is set up to let the whisper engine know
you intend to use the trinity synth. 

### whisper\_trinity\_noteon

    EXPORT void whisper_trinity_noteon(int voice);

Sends a noteon message to a particular voice.  

### whisper\_trinity\_noteoff

    EXPORT void whisper_trinity_noteoff(int voice); 

Sends a note off message to a particular voice.  

### whisper\_trinity\_pitch

    EXPORT void whisper_trinity_pitch(int voice, int nn);

This sets the pitch of a particular note. The pitch it is set to is a 
MIDI note number (MIDI note 60 = Middle C4).

### whisper\_trinity\_voice\_state

    EXPORT void whisper_trinity_voice_state(int instr, int voice, int state);

This function sets the particular state of a voice in Trinity. When the state
is 0, the voice is turned off, and when it is 1, it is turned on. Turning 
off a voice will make it so that particular voice is not computed. This is
useful to do for running on computers with weak processing power.

Note: By default, only voice 0 is turned on. Voices must explicitely be turned 
on in order to do polyphony.

The instrument parameter *instr* refers to which instrument is being 
referred to.

### whisper\_trinity\_default

    EXPORT void whisper_trinity_default(int instr);

This sets a particular trinity instrument to the default sound.


### whisper\_trinity\_legato(int instr, int mode);

    EXPORT void whisper_trinity_legato(int instr, int mode);

This turns on or off  legato mode on a particular instances of Trinity. A mode
of "0" will turn it off, and a mode of "1" will turn it on.
Legato mode causes notes to glissando (slide) between pitches rather than 
immediately jump.  It is very suitable for monophonic patches.

Legato mode is implemented a bit differently compared to most how commercial 
synthesizers do it, so some explanation is required for those familiar with the 
concept. In Trinity, legato notes happen relative to a particular voice. That 
is to say, a note played on a voice will only gliss if the pitch is changed 
on for that voice. Because of this property, it is possible for Trinity to
play multiple legato lines at once.


### whisper\_trinity\_port\_time

    EXPORT void whisper_trinity_port_time(int instr, SPFLOAT ptime);

This function will set the portamento time for a given note. The portamento 
time controls the time it takes to gliss (slide) from one note to another when
the note is in legato mode. The unit of portamento time is in seconds.


## Oscillator Control

An oscillator is a sound generator that produces the core sound before it
is shaped by filters and envelopes. It is tuned to play at a frequency based on 
the current voices pitch. Each voice in Trinity has access to 3 oscillators 
that can be played (hence the name). Each oscillator is given an id from 0
to 2. The functions below are used to modify a specific oscillator number. 
Note that these functions touch every voice of the current instrument. For 
example, changing the detune amount on oscillator 0 would change the detune
amount for oscillator 0 on every voice. 


### whisper\_trinity\_osc\_sine

    EXPORT void whisper_trinity_osc_sine(int instr, int osc);

This sets the oscillator waveform type to be a sine wave. 

### whisper\_trinity\_osc\_saw

    EXPORT void whisper_trinity_osc_saw(int instr, int osc);

This sets the oscillator waveform type to be a saw wave. 

### whisper\_trinity\_osc\_toggle
    
    EXPORT void whisper_trinity_osc_toggle(int instr, int osc, int state);

This function turns an oscillator on or off. A state of 1 is on, and 0 
is off. When the oscillator is turned off, the oscillator will not compute
any sound, which is useful for on machines where processing power is low.

### whisper\_trinity\_osc\_detune

    EXPORT void whisper_trinity_osc_detune(int instr, int osc, SPFLOAT detune);

This sets the detune amount of a given oscillator. The detune parameter is 
a floating point parameter (by default, SPFLOAT just a float), whose units 
are in relative MIDI note number values. For instance, a value of 0.5 will 
cause an oscillator to be sharp but half a semitone. A value of -2 will cause
the detune to be a whole step below. Typically, detune values are very 
small (less than 0.1, positive or negative). These small values cause a 
chorusing effect which can create a thicker sound.

 

### whisper\_trinity\_osc\_transpose

    void whisper_trinity_osc_transpose(int instr, int osc, int trans);

This will transpose an oscillator by a whole integer amount in relative MIDI 
note number units.  A value of 12 will transpose up an octave. A value of -12 
is down an octave.  +/- 7 will transpose by a fifth.

## Filter control

In subtractive synthesis, a signal generated by the oscillator is fed into
a filter of some sort, where harmonics in the signal are "subtracted" to sculpt
the timbre of the sound.

Trinity is designed to work with a variety of filters, one filter at a time. 
Filters are typically some sort of lowpass filter, but they can also be bandpass
or highpass filters.

Each voice in Trinity has it's own filter instance. This is done so that there
can be independent filter control for each voice via an envelope.


### EXPORT void whisper\_trinity\_cutoff

    EXPORT void whisper_trinity_cutoff(int instr, SPFLOAT cutoff);

This function sets the overall cutoff frequency of the filter in Hz for a 
particular Trinity instrument. 

### EXPORT void whisper\_trinity\_res

    EXPORT void whisper_trinity_res(int instr, SPFLOAT res);

This will set the resonance for a filter. This sould be a value between 0 and 1.
This parameter will not do anything for filters that do not implement resonance.

### EXPORT void whisper\_trinity\_filter\_moog

    EXPORT void whisper_trinity_filter_moog(int instr);

This will set the filter type to be a moog ladder filter emulation. 

### EXPORT void whisper\_trinity\_filter\_butlp

    EXPORT void whisper_trinity_filter_butlp(int instr);

This will set the filter type to be a butterworth lowpass filter. 

## Envelopes

Envelopes are time-based line generators that control the expression of a 
given parameter. They are tightly linked with note on and note off events. 
Trinity has two envelopes, one for for amplitude, and one for filter.
The amplitude envelope controls the overall volume, and therefore
shapes the transient shape of the note.
The filter envelope controls the filter cutoff frequency, and therefore shapes
the timbral evolution of the note.

The envelopes here use an ADSR envelope, which stands for Attack, Decay, 
Sustain, and Release. This is a classic kind of envelope used in synthesis, and
implemented on most commercial synthesizers. The ADSR envelope is a line segment
with 4 states:

- The *attack* state happens at the start of a note. This typically rises from
0 to some target value over some duration of time. 
- The *decay* state happens after the attack state is finished. It starts 
at the attack state level, and falls to the sustain level over some period 
of time.
- The *sustain* state happens after the decay state. It is held indefinitely
until there is a noteoff message.
- The *release* state happens after a noteoff message. It starts at whatever
the previous level was, and falls to zero over some period of time. This state
can be reached to from either the *attack*, *decay*, or *sustain* states.


### EXPORT void whisper\_trinity\_amp\_atk

    EXPORT void whisper_trinity_amp_atk(int instr, SPFLOAT val);

This sets the amplitude envelope attack time (in seconds).

### EXPORT void whisper\_trinity\_amp\_dec

    EXPORT void whisper_trinity_amp_dec(int instr, SPFLOAT val);

This sets the amplitude envelope decay time (in seconds).

### EXPORT void whisper\_trinity\_amp\_sus

    EXPORT void whisper_trinity_amp_sus(int instr, SPFLOAT val);

This sets the amplitude envelope sustain amount. This should be a value between
0 and 1.

### EXPORT void whisper\_trinity\_filt\_rel

    EXPORT void whisper_trinity_filt_rel(int instr, SPFLOAT val);

This sets the filtlitude envelope release time (in seconds).

### EXPORT void whisper\_trinity\_filt\_atk

    EXPORT void whisper_trinity_filt_atk(int instr, SPFLOAT val);

This sets the filter envelope attack time (in seconds).

### EXPORT void whisper\_trinity\_filt\_dec

    EXPORT void whisper_trinity_filt_dec(int instr, SPFLOAT val);

This sets the filter envelope decay time (in seconds).

### EXPORT void whisper\_trinity\_filt\_sus

    EXPORT void whisper_trinity_filt_sus(int instr, SPFLOAT val);

This sets the filter envelope sustain amount. This should be a value between
0 and 1.

### EXPORT void whisper\_trinity\_filt\_rel

    EXPORT void whisper_trinity_filt_rel(int instr, SPFLOAT val);

This sets the filter envelope release time (in seconds).


### EXPORT void whisper\_trinity\_filter\_amt

    EXPORT void whisper_trinity_filter_amt(int instr, SPFLOAT val);

This sets the amount of influence the filter envelope has on the cutoff
frequency. This expects a value between 0 and 1. A value of 0 is no effect,
while a value of 1 will set the range of control to be from 0Hz to the cutoff.
A value of 0.5 will only do half control, producing an envelope signal going
from half the cutoff frequency to the cutoff frequency.


## Things To Implement

Here is a list of things that should easily be added onto the current trinity
setup without too much trouble:



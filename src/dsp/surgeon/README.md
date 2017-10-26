# Surgeon

Four-operator FM synthesizer. Surgeon was the third synthesizer built for
EyeJam. Compared to [Trinity](../trinity/README.md), the interface is somewhat
cleaner internally. Because the general eyejam clip launching interface was
completed at this point, there was never an effort to play surgeon as a 
standalone. 

Surgeon employs a computationally efficient synthesis technique known as 
Frequency Modulation (FM) Synthesis. Conceptually, FM synthesis can be thought
of as very very fast vibrato. In classical FM synthesis, the frequency
of a sinusoidal signal (known as a carrier signal), is modulated by another 
sinusoidal signal (known as the modulator signal). When the frequency of 
modulator is audio rate (approximately 20Hz or more), the modulation introduces 
side band frequencies which produce a spectrum. When the frequency of the 
modulator is sub-audio rate (less than about 20Hz), the sound percieved as
some sort of vibrato. 

At the time of writing, these functions don't actually exist yet, but the
prototypes are in place in the main [dsp header file](../dsp.h). 


## Instantiation 

### surgeon\_instr\_size();

    size_t surgeon_instr_size();

This returns the size of the `surgeon_inst` data struct, which is an 
opaque pointer. This is most likely used with malloc:

    surgeon_instr *instr = malloc(surgeon_instr_size());
    /* do stuff here */
    free(instr)

### surgeon\_instr\_init

    void surgeon_instr_init(sp_data *sp, surgeon_instr *surg);

Initialize data inside of surgeon. Surgeon  uses Soundpipe under the hood,
so sp\_data is an argument. Note that stuff does get allocated with malloc in
this function, so only call this once.


### surgeon\_instr\_destroy

    void surgeon_instr_destroy(surgeon_instr *surg);

This destroys all data allocated from calling `surgeon_instr_init`.

## Note Handling

Each instrument has its own set of voices to control. These voices go from 
0 to SURGEON\_MAXVOICES - 1.

### surgeon\_noteon

    EXPORT void surgeon_noteon(surgeon_instr *instr, int voice);

This turns on a particular voice in a particular instrument instance of 
surgeon. 

### surgeon\_noteoff

    EXPORT void surgeon_noteoff(surgeon_instr *instr, int voice);


This turns off a particular voice in a particular instrument instance of 
surgeon.

### surgeon\_setnote

    EXPORT void surgeon_setnote(surgeon_instr *instr, int voice, int nn);

This sets the note (in MIDI units) for a particular voice of a particular 
instrument.

## Operator 

An operator is a single sinusoidal oscillator inside of a Surgeon voice. 
There are 4 operators in a given voice. Operators can be used to modulate 
the frequency of other operators. These operators can be routed and configured
to produce a wide range of timbres.

The operator functions below contain parameters for both the instrument id 
**instr**, and the oscillator/operator number **osc**. An assumption made is that
a polyphonic surgeon instrument will have the operator parameters for each voice.
For this reason, voice number is excluded here. Every operater parameter change
will touch each voice instance. 

### surgeon\_op\_mode

    EXPORT void surgeon_op_mode(surgeon_instr *instr, int osc, int mode);

The mode of an operator changes the internal behavior of an operator. An 
operator has two major parameters: frequency and amplitude. Either of these
can be set to a fixed or relative mode. A fixed frequency or amplitude are
absolute internal values. For frequency, this is a value in Hertz. For 
amplitude, this is a linear scaling value: a typical range is between 0 and 1. 
Absolute amplitude is used if a signal is meant to be a carrier signal (coming
from the speakers), rather than a modulator signal.

Relative values are based off of the current note being played in the voice
the operator is attached to. A frequency is said to be have the frequency 
*scaled* by some value. The relative amplitude also factors in frequency of
the voice. It is known and the modulation index, or *index*.

There a total of 4 combinations possible, outline in the following 4 modes:

- mode 0: fixed frequency and amplitude
- mode 1: relative frequency and fixed amplitude 
- mode 2: fixed frequency and relative amplitude (modulation index)
- mode 3: relative frequency and relative amplitude (modulation index)


### surgeon\_op\_freq

    EXPORT void surgeon_op_freq(surgeon_instr *instr, int osc, SPFLOAT freq);

This sets the absolute frequency of an operator.  This value is only used when 
the operator mode is set to play a fixed frequency (mode 0 or 2).

### surgeon\_op\_amp

    EXPORT void surgeon_op_amp(surgeon_instr *instr, int osc, SPFLOAT amp);

This sets the absolute amplitude of an operator. This value is only used when
the operator mode is set to play a fixed amplitude (mode 0 or 1).

### surgeon\_op\_index

    EXPORT void surgeon_op_index(surgeon_instr *instr, int osc, SPFLOAT index);

This sets the modulation index of a given operator, which is a relative
amplitude. This value only is set when the mode is either 2 or 3.

### surgeon\_op\_scale

    EXPORT void surgeon_op_scale(surgeon_instr *instr, int osc, SPFLOAT scale);

This sets the scaling value for a frequency. This only works when the frequency
is set to be relative (mode 1 and 3). 

### surgeon\_algo

    EXPORT void surgeon_algo(surgeon_instr *instr, int algo);

An "algorithm" is a term from the DX7 FM syntheiszer, and refers to the 
given operator configuration. The number of algorithms is determined by the 
macro SURGEON\_NALGO. At the time of writing there are 3 algorithms:

- algorithm 0: nothing. All operators off. computes silence.
- algorithm 1: simple. This turns on operator 0 only.
- algorithm 2: classic. Implements a classic FM operator pair with operators 0
and 1. Operator 0 is the carrier, and operator 1 is the modulator.


## Envelopes
    
Every operator has it's own envelope that modulates the amplitude. When 
the operator is set to be a carrier signal (heard by the speaker), this envelope
functions as an amplitude envelope. When the operator is set to be a modulator 
(modulating the frequency of another operator), the envelope functions as
a timbral envelope.

The envelope used is a classic ADSR envelope, with time segments that determine 
attack, decay, sustain, and release. For more information on envelopes, see
the [envelope section found in trinity](../trinity/README.md#envelopes).

Like the previous section on general operator control, these functions control
one of the four operators in a given voice. Since the assumption is that all
voice parameters will be the same, that has been excluded. A value changed for
a particular operator number will change that parameter for all voices.

### surgeon\_op\_atk

    EXPORT void surgeon_op_atk(surgeon_instr *instr, int osc, SPFLOAT val);

Sets the attack time, in seconds.

### surgeon\_op\_dec

    EXPORT void surgeon_op_dec(surgeon_instr *instr, int osc, SPFLOAT val);

Sets the decay time, in seconds.

### surgeon\_op\_sus

    EXPORT void surgeon_op_sus(surgeon_instr *instr, int osc, SPFLOAT val);

Sets the sustain level. Should be a value in the range 0-1.

### surgeon\_op\_rel

    EXPORT void surgeon_op_rel(surgeon_instr *instr, int osc, SPFLOAT val);

Sets the release time, in seconds.

### surgeon\_op\_amount

    EXPORT void surgeon_op_amount(surgeon_instr *instr, int osc, SPFLOAT val);

Sets the amount of influence the envelope has on the amplitude. This is a value
in the range 0-1. A value of 1 is full, a value of 0.5 is 50%, and a value of
0 is no effect.


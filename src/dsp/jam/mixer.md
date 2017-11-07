# Mixer

In MSHFSJ, the mixer is the component analogous to a mixer you would find
in a conventional digital audio workstation. The task of the mixer is to sum
all the tracks together into one signal and handle any global effects 
processing.

The mixer layer thinly abstracts away the computation portion for tracks, 
summing them all to a stero signal. The reason for this abstraction is for
global effects such as reverb or delay using what are known as *sends*.

## Sends
Each track is able to contribute 
some amount to the global effects. This is known as *send*, and it is 
represented internally as a multidimensional array of SPFLOATS. The following
function is used to control how much a track contributes to a particular send
channel:

    void whisper_mixer_send(int track, int send, SPFLOAT amt);

Where **track** is a zero-index track number, **send** is a zero index send
number, and **amt** is the amount in dB to send. A value of 0 is considered
"fullscale", amplification is above 0, and attenuation is anything below 0. 
If a value goes below -100dB, it is automatically clipped to be 0, effectively
muting the effect send for that track.

## Effects

The mixer has a variety of global effects. An effect in whisper has a special
kind of interface for computation. There are ultimately 3 kinds of 
callbacks needed for computation:

1. initialization: this function allocates and initializes memory for the 
effect. The sole parameter needed here is sampling rate. Effects using soundpipe
can either use their own soundpipe data structure or use the global soundpipe
data struct via ``whisper_sp_data()``

2. computation: an interface for computing a single frame of audio. The hard 
coded assumption here is that inputs are mono sources, and outputs are stereo.
This also passes the void pointer containing any allocated data.

3. destruction: free any memory previously allocated.

In addition to these core callbacks, an additional function will be needed for
binding user data. 


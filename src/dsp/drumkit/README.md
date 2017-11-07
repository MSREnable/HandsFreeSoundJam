# Drumkit

A sample-based drumkit that is a part of EyeJam. 
This instrument will load single-shot drum samples into
memory to be played back later. Internally, this instrument uses the
single-header public domain library 
[DrFlac](https://github.com/mackron/dr_libs/blob/master/dr_flac.h) 
to load samples into memory. These samples can then be called back on demand.
When [sequencing note information with clips](../jam/clip.md), samples 
correspond to midi note numbers. The default instrument sounds have the 
following associations:
    - note 0: kick
    - note 1: snare
    - note 2: closed high-hat

The drumkit has a finite number of voices and samples. The number of voices
is determined by the macro DK\_NVOICES. The number of samples is determined by
the macro DK\_NSAMPLES. These macro definitions can be found in the top of
[drumkit.c](drumkit.c). A voice can play a single sample simultaneously, and
there can be more samples than voices. A clever composer could be able to play
a wide variety of samples with a small amount of voices.

This drumkit expects to find drum samples in a folder called "samples". 
This [directory is included in Eyejam](../samples), and should be copied into 
the same directory as the executable. If the samples are not found, the drum
kit will run, but will only produce silence.
Note that the file paths are relative and not absolute. 
In some eye-controlled interfaces such as Communicator 5,
you will need to launch the eyejam executable using a batch script rather
than from the executable directly.  

Future plans for the DrumKit include an interface where custom drum samples can
be loaded and varispeed control.  

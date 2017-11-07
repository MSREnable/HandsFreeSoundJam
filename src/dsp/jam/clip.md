# Clips


## Introduction 

Clips are the atomic particle which ties all of MSHFSJ together. Conceptually,
a "clip" is a short musical phrase. Clips are launched via the MSHFSJ interface
to build up a piece of music in realtime. Clips, and clip launching in 
MSHFSJ are somewhat analogous to the features found in the 
digital audio workstation 
[Ableton Live](https://en.wikipedia.org/wiki/Ableton_Live).

## A bit deeper

Compared to similar formats storing musical data like 
[MIDI](https://en.wikipedia.org/wiki/MIDI#Standard_files)
or 
[MusicXML](https://en.wikipedia.org/wiki/MusicXML)
, the 
`whisper_clip` data type is rather primitive. It is most analogous in 
structure to the way note information in 
[the module file format](https://en.wikipedia.org/wiki/Module_file) found in
[music trackers](https://en.wikipedia.org/wiki/Music_tracker).

Note information is stored in a 
spreadsheet-like table of data. Each column denotes a note, while each row
denotes a step in time. Columns are known as *voices*, and the number of rows 
determines the *length* of a clip. 
Each row is an equal step in time, typically at the 
musical resolution of a 16th note. In order for a clip to be played, it needs a 
clock source. At each clock tick, the clip steps to the next row, reading and
updating note information that can be read by internal synthesizers, such
as [trinity](../trinity/README.md), [surgeon](../surgeon/README.md), or 
[drumkit](../drumkit/README.md). 

The design is very simple, but there are a few drawbacks. Timing resolution
is limited to 16th note resolutions. Polyphony is limited by the number of 
columns that the table has. This approach requires more memory usage than the
more complicated alternatives. The maximum limits for voices and length are hard 
coded in [tracks.c](tracks.c) as `CLIP_MAXVOICES` and `CLIP_MAXLEN`. Be wary
of changing these, as they may have unintended consequences for the 
loading/saving to the [database](db.md).


## Clip Parameters

The Whisper API has an interface for editing clips designed to be easily used 
from the C# level. These functions are fairly low level. Higher-level C# 
abstractions built on top of these functions make sense.

To edit a particular clip, one needs to know the track number and the clip id
associated with that track. Many of the operations also work on a single note,
so position of the note is also needed, as well as the voice channel (column).


### whisper\_clip\_set\_note

    EXPORT void whisper_clip_set_note(int track, int clip, int pos, int voice, int nn);

This sets a note for a particular voice. Note is a MIDI note number.

### whisper\_clip\_set\_length

    EXPORT void whisper_clip_set_length(int track, int clip, int length);

Sets the clip length.

### whisper\_clip\_set\_gate

    EXPORT void whisper_clip_set_gate(int track, int clip, int pos, int voice, int gate);

Sets the clip gate. Alternative gate messages can enable things like legato 
behavior.

### whisper\_clip\_set\_arg

    EXPORT void whisper_clip_set_arg(int track, int clip, int pos, int voice, int arg, int value);

Sets a note argument. Most of these don't do anything yet. 

### whisper\_clip\_set\_nvoices

    EXPORT void whisper_clip_set_nvoices(int track, int clip, int nvoices);

Sets the number of currently active voices on a clip. 

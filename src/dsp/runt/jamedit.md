# Sound Jam Editor

The sound jam editor is a small scripting language built for quickly 
constructing songs that can be loaded up inside of Sound Jam. The 
scripting language is built using the public domain stack-based interpreter 
[Runt](https://paulbatchelor.github.com/proj/runt). The jam editor implements
a set of words which are bound to the API used internally to control the sound
software. 

## Compilation 

[Runt will need to be compiled and installed before going further.](
https://www.github.com/paulbatchelor/runt.git)

On Linux, the Sound Jam Editor can be compiled by running:

    make je

Which is a shortcut for this:

    make -f Makefile -f Makefile.linux jamedit

To compile for other operating systems, replace the "Makefile.linux" with
the Makefile for that particular target.

This will create the "jamedit" executable.

## Quick Start

The following code will erase then sequence a major scale on track 0, 
clip 0 (the melody track), and save the note information to file: 

    1 load_song

    0 set_track
    0 set_clip
    2 set_notelen
    60 set_base

    erase

    0 insert
    2 insert
    4 insert
    5 insert
    7 insert
    9 insert
    11 insert
    12 insert

    1 save_song

This code can be saved to a file "notes.rnt" and then run on the commandline

    ./jamedit notes.rnt

This will write to or create a file called data.db, a SQLite databse 
which Sound Jam will use to load pattern data.

## Word Reference 

The following words are implemented to help edit clips in Sound Jam.

### insert

Usage:

    note insert

Inserts a note at the current edit cursor position. The note is a number 
that is relative to base note. If the base note were 60 (middle C), then
`0 insert` would insert C, `2 insert` would insert D, `-1 insert` would insert
B, etc.


### save_song

Usage:

    song_id save_song

Saves a song to disk with the song_id `song_id`. A song_id of 1 is the default
song slot that Sound Jam opens by default.

### load_song

Usage:

    song_id load_song

This word will attempt to load a song from file with a numerical 
song id song_id. If the song doesn't exist, it will issue a warning on
the terminal.

### set_clip_length
Usage:

    length set_clip_length

Sets the length of the currently selected clip, set by `set_clip`, and
`set_track`.


### loopmode

Usage:
    
    mode loopmode

Turns on loopmode for the currently selected track, which can be set by 
`set_track`.

### set_notelen

Usage:

    length set_notelen

Sets the note length of the edit cursor. These must be whole integer numbers. 
A length of 4 is a quarter note, a length of 1 is a sixteenth note, and a 
length of 8 is a half note.

### set_track

Usage:

    track_num set_track

Sets the current track to be a particular track number. Should be in range
0-4.

### set_clip

Usage:

    clip_num set_clip

Sets the current clip to be a particular clip number. Should be in range
0-4.

### set_pos

Usage:

    position set_pos

Sets the edit cursor position.

### voice_up

Usage:

    voice_up

Sets the current voice channel to go up by one.

### voice_down

Usage:

    voice_down

Sets the current voice channel to go down by one.

### stepsize_double

Usage:

    stepsize_double

Doubles the current edit step size and note insertion duration.

### stepsize_half

Usage:

    stepsize_half

Halves the current edit step size and note insertion duration.

### set_base

Usage:

    base set_base

Set the base note for the edit step inserter. This is a MIDI note number. 
60 is middle C.

### trinity_preset

Usage:

    preset instr trinity_preset 

Selects a preset sound from the built-in presets. `preset` should be value
approximately in range 0-10, and `instr` should either be 0 or 1. Trinity 0
controls the "Melody" instrument, and Trinity 1 controls the "Bass" instrument
coupled with the "Chords". 

### surgeon_preset

Usage:

    preset instr surgeon_prese

Selects a preset sound from the built-in presets. `preset` should be value
approximately in range 0-10, and `instr` should either be 0 or 1. Surgeon 0
controls the one of "Chords" instruments, and Trinity 1 controls the "Melody 2" 
instrument.

### set_title

Usage:

    "title" set_title

Sets the song title of the current song. The character limit for the song is
40 characters.

### set_tempo

Usage:

    tempo set_tempo

Sets the song tempo, in beats-per-minute (BPM).

### erase

Usage:

    erase

Erases the note information in the currently selected clip. 

### step_left

Usage:

    step_left

Moves the edit cursor to the left. 

### step_right

Usage:

    step_right

Moves the edit cursor to the right. 

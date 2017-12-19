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

### save_song

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

### set_track

### set_clip

### set_pos

### voice_up

### voice_down

### stepsize_double

### stepsize_half

### set_base

### trinity_preset

### surgeon_preset

### set_title

### set_tempo

### erase

### step_left

### step_right

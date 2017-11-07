# The Clip Editing Interface

The following document will describe the clip editing interface used
by MSHFSJ. 

The clip editing interface was designed to provide a means for efficiently
inserting and removing notes in a clip. This editor makes a few good musical 
assumptions: 

1. the range of notes for a given melody will be about an octave
2. note durations will by and large be more similar than different
3. durations and rhythms will most often share power-of-2 relationships 
(halving/doubling)

For these reasons, the clip editing interface imposes a lot of implicit 
behavior. That is to say, octave range and duration must be set before
inserting a given note.

When in doubt, consult the source. Most of these functions are used 
in the 
[piano](../../../Fidget/src/eyejam/piano.c),
[piano roll](../../../Fidget/src/eyejam/pianoroll.c),
and 
[editor](../../../Fidget/src/eyejam/editpanel.c)
UI components for MSHFSJ.

## whisper\_eyejam\_edit\_init
    EXPORT void whisper_eyejam_edit_init();

This function should be called before attempting to use the edit interface.
This will zero out and set sane defaults for variables. 

## whisper\_eyejam\_edit\_set\_track
    EXPORT void whisper_eyejam_edit_set_track(int track);

Sets the specific track currently being edited. 

## whisper\_eyejam\_edit\_get\_track
    EXPORT int whisper_eyejam_edit_get_track();

Gets the current track number being edited.

## whisper\_eyejam\_edit\_set\_clip
    EXPORT void whisper_eyejam_edit_set_clip(int clip);

Sets the current clip number being edited, relative to the track.

## whisper\_eyejam\_edit\_get\_clip();
    EXPORT int whisper_eyejam_edit_get_clip();

Gets the current clip number being edited, relative to the track.

## whisper\_eyejam\_edit\_set\_base
    EXPORT void whisper_eyejam_edit_set_base(int base);

Sets the base MIDI note to use. Notes inserting will be relative to this
note. The default is 60.

## whisper\_eyejam\_edit\_get\_base
    EXPORT int whisper_eyejam_edit_get_base();

Gets the current base MIDI note to use. 

## whisper\_eyejam\_edit\_set\_voice
    EXPORT void whisper_eyejam_edit_set_voice(int voice);

Sets the current voice channel to write to in the currently selected clip.

## whisper\_eyejam\_edit\_get\_voice
    EXPORT int whisper_eyejam_edit_get_voice();

Gets the current voice channel in the currently selected clip.

## whisper\_eyejam\_edit\_nextnote
    EXPORT void whisper_eyejam_edit_nextnote();

Jumps to the next note in the current clip's voice channel.

## whisper\_eyejam\_edit\_prevnote
    EXPORT void whisper_eyejam_edit_prevnote();

Jumps to the previous note in the current clip's voice channel.

## whisper\_eyejam\_edit\_set\_pos
    EXPORT void whisper_eyejam_edit_set_pos(int pos);

Sets the current edit position in the clip.

## whisper\_eyejam\_edit\_get\_pos
    EXPORT int whisper_eyejam_edit_get_pos();

Gets the current edit position in the clip.

## whisper\_eyejam\_edit\_step\_left
    EXPORT void whisper_eyejam_edit_step_left(); 
Moves the edit position to the left. The step size is determined by the current 
notelen value.

## whisper\_eyejam\_edit\_step\_right
    EXPORT void whisper_eyejam_edit_step_right();

Moves the edit position to the right. The step size is determined by the current 
notelen value.

## whisper\_eyejam\_edit\_set\_notelen
    EXPORT void whisper_eyejam_edit_set_notelen(int notelen);

Sets the current note length, which is also the edit step size for navigation. 

## whisper\_eyejam\_edit\_get\_notelen
    EXPORT int whisper_eyejam_edit_get_notelen();

Gets the current note length, which is also the edit step size for navigation. 

## whisper\_eyejam\_edit\_insert
    EXPORT void whisper_eyejam_edit_insert(int nn);

Inserts a note at the current edit position. The note number is relative to
the base note number. If the base is 60, a value of 1 would be 61, -2, would 
be 58, etc...

## whisper\_eyejam\_edit\_clear
    EXPORT void whisper_eyejam_edit_clear();

Clears all note information in the current clip.

## whisper\_eyejam\_edit\_remove
    EXPORT void whisper_eyejam_edit_remove();

Removes note at the current playhead position. Note that the edit cursor
needs to be set to be at the beginning of the note for it to actually be
removed. Otherwise, nothing will happen.

## whisper\_eyejam\_edit\_transpose
    EXPORT void whisper_eyejam_edit_transpose(int transpose);

Transpose bias by given amount. 12 is up an octave, -12, is down an octave,
+/-7 is up/down a fifth, etc...

## whisper\_eyejam\_edit\_nextclip
    EXPORT void whisper_eyejam_edit_nextclip();

Goes to the next clip in the currently selected track/

## whisper\_eyejam\_edit\_prevclip
    EXPORT void whisper_eyejam_edit_prevclip();

Goes to the previous clip in the currently selected track/

## whisper\_eyejam\_edit\_play
    EXPORT void whisper_eyejam_edit_play();

Schedules current clip to be played. This basically functions as a clip 
launcher.

## whisper\_eyejam\_edit\_clip\_length
    EXPORT int whisper_eyejam_edit_clip_length();

Gets length of current clip.

## whisper\_eyejam\_edit\_clip\_nvoices
    EXPORT int whisper_eyejam_edit_clip_nvoices();

Gets the number of voices in the clip.

## whisper\_eyejam\_edit\_clip\_note
    EXPORT int whisper_eyejam_edit_clip_note(int pos, int voice);

Gets note information for a given position and voice channel.

## whisper\_eyejam\_edit\_clip\_gate
    EXPORT int whisper_eyejam_edit_clip_gate(int pos, int voice);

Gets gate information for a given given position and voice channel.

## whisper\_eyejam\_edit\_isprevnote
    EXPORT int whisper_eyejam_edit_isprevnote(int note);

Checks if given note is a PREVNOTE macro.

## whisper\_eyejam\_edit\_stepsize\_up
    EXPORT void whisper_eyejam_edit_stepsize_up();

Increases step size (and note length) by one.

## whisper\_eyejam\_edit\_stepsize\_down
    EXPORT void whisper_eyejam_edit_stepsize_down();

Decreases step size (and note length) by one.

## whisper\_eyejam\_edit\_stepsize\_double
    EXPORT void whisper_eyejam_edit_stepsize_double();

Doubles current step size.

## whisper\_eyejam\_edit\_stepsize\_half
    EXPORT void whisper_eyejam_edit_stepsize_half();

Halves current step size.

## whisper\_eyejam\_edit\_voice\_up
    EXPORT void whisper_eyejam_edit_voice_up();

This moves the voice channel up by one.

## whisper\_eyejam\_edit\_voice\_down
    EXPORT void whisper_eyejam_edit_voice_down();

This moves the voice channel down by one.

## whisper\_eyejam\_edit\_track\_next
    EXPORT void whisper_eyejam_edit_track_next();

Selects the the next track.

## whisper\_eyejam\_edit\_track\_prev
    EXPORT void whisper_eyejam_edit_track_prev();

Selects the the previous track.

## whisper\_eyejam\_edit\_playhead
    EXPORT double whisper_eyejam_edit_playhead();

Gets current playhead position as a normalized value from 0 to 1. 

## whisper\_eyejam\_edit\_playing
    EXPORT int whisper_eyejam_edit_playing();

Checks if the current clip is being played. 1 = yes, 0 = no.

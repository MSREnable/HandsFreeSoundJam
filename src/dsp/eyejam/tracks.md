# Tracks

In EyeJam, a **track** binds a group of [clips](clip.md) to a synthesizer
instrument like [trinity](../trinity/README.md), 
[surgeon](../surgeon/README.md), or [drumkit](../drumkit/README.md).


## Exported Functions 

For the most part, it isn't necessary to directly access track data. Most of 
the work is done for you.
However, a few functions have been exported, mostly for UI related things.
The exported track functions provided are designed to be used with UI elements.


### whisper\_tracks\_loopmode

    EXPORT void whisper_tracks_loopmode(int track, int mode);

The function *whisper_tracks_loopmode* turns on the loop mode for the clips
associated with the track. When the mode is 1, looping is turned on. When
the mode is 0, the mdoe is turned off.


### whisper\_tracks\_gain

    EXPORT void whisper_tracks_gain(int track, SPFLOAT gain);

The function *whisper_tracks_gain* sets the overall gain of a particular track
in dB units. A value of "0" is full scale. 


### whisper\_tracks\_current\_clip

    EXPORT int whisper_tracks_current_clip(int track);

This function returns the current clip being played. If the track is currently 
playing, the return value is an integer between 0 and ``WHISPER_NCLIPS - 1``. 
If the track is not currently playing a clip, it will return a negative value.


### whisper\_tracks\_next\_clip

    EXPORT int whisper_tracks_next_clip(int track);

This function returns the id of next clip queued up. An id value of 0
and ``WHISPER_NCLIPS - 1`` is returned if a clip is queued up. Otherwise, it
returns a negative value.


### whisper\_tracks\_get\_row\_id

    EXPORT int whisper_tracks_get_row_id(int track);

This function returns the unique row id associated with the SQLite table that
stores track data. If there is unique row id assigned to it, the row id is
a negative value less than 0.


### whisper\_tracks\_set\_row\_id

    EXPORT void whisper_tracks_set_row_id(int track, int id);

This function sets the unique row id of the track. This id corresponds to 
the SQLite table row that stores track data.


### whisper\_tracks\_get\_clip\_row\_id

    EXPORT int whisper_tracks_get_clip_row_id(int track, int clip);

This function returns the unique clip id inside of a particular track. 



### whisper\_tracks\_get\_playhead

    EXPORT double whisper_tracks_get_playhead(int track);

This function returns the playhead position of a given track, normalized to
be between 0 and 1. The resolution of playhead is at the tick level, 
which is some subdivision of the 16th note. This subdivision is determined by
``WHISPER_NTICKS``. 


### whisper\_tracks\_am\_i\_alive

    EXPORT int whisper_tracks_am_i_alive(int track);

Returns the playback status of a given track. If clip is playing or is about
to be played, it will return a value of 1. Otherwise, it will return a value of
0.

# Database Management

This README will describe persistent and database management. 

## A summary of data hierarchy in Eyejam

Data in EyeJam stores data using the SQLite database format. SQLite is a 
cross-platofrm battle-tested flexible format that is more than suitable for
the needs of EyeJam. Right now, the SQLite database is used to store
musical note information for **clips**, **tracks**, and **songs**. 

The core atomic structure of the musical note information is a [clip](clip.md),
which contains note information, stored in a way that is very similar to
how classic music trackers store information. Clips themselves do not produce 
sound, but they can be read by a synthesizer through a **track**.

A group of five clips are bound together in a data element known as a 
[track](tracks.md). A track will take the note information, and use it to control
a synthesizer such as [Trinity](../trinity/README.md). Tracks also can do
things like send audio to global sends in the [mixer](mixer.md), but this is
currently out of scope for what SQLite is able to save.

A group of tracks (the number of which is hard coded to be 
``WHISPER_NTRACKS``) are grouped in a high-level interface which will be 
referred as a **song**. This is a singular unit which handles all the musical 
note in a single instance. This abstraction is desirable should EyeJam want
to be able to load and store multiple songs in the SQLite database. 
Note that a song only handles note metadata. For full granularity of control, 
more tables will need to be made to handle things like synth and effect 
presets. 

## API reference

The following functions are public facing functions that allow easy access
to persistent data.

### whisper\_eyejam\_db\_open
    EXPORT void whisper_eyejam_db_open(const char *db);

Opens the database. A filename must be supplied here. In practice, this 
has just been "data.db". 

### whisper\_eyejam\_db\_close
    EXPORT void whisper_eyejam_db_close();

Closes the database.

### whisper\_eyejam\_db\_load\_default
    EXPORT void whisper_eyejam_db_load_default();

Loads the default song, tracks, and clips from the database. 

### whisper\_eyejam\_db\_save\_default
    EXPORT void whisper_eyejam_db_save_default();

Saves the current song, tracks, and clips from the database.

### whisper\_eyejam\_db\_save\_clip
    EXPORT void whisper_eyejam_db_save_clip(int track, int clip, int id);

Saves a clip number *clip* from a particular track number *track* to file
with an id of *id*. If the ID is negative, an ID is automatically assigned.
Any existing ID in that slot is overwritten. 


### whisper\_eyejam\_db\_load\_clip
    EXPORT void whisper_eyejam_db_load_clip(int track, int clip, int id);

Loads a clip from file with the id *id* to a particular clip *clip* in track
*track*. 

### whisper\_eyejam\_db\_save\_track
    EXPORT void whisper_eyejam_db_save_track(int track, int id);

Saves a track to a location with an id of *id*. If id is -1, a new track ID
will be automatically assigned. This will implicitely save clips in each of the 
tracks to unique positions. 


### whisper\_eyejam\_db\_load\_track
    EXPORT void whisper_eyejam_db_load_track(int track, int id);

This will load a track, and the underlying clips, from a file with the track
id *id*.

### whisper\_eyejam\_db\_save\_song
    EXPORT void whisper_eyejam_db_save_song(int id);

This will save a song to a particular song id *id*. This will implicitely save
unique entries for tracks, which in turn will save unique entries for clips.

### whisper\_eyejam\_db\_load\_song
    EXPORT void whisper_eyejam_db_load_song(int id);

This will load a saved song with song id *id*, along with all the track 
and clip information that comes along with it.

### whisper\_eyejam\_db\_exec
    EXPORT void whisper_eyejam_db_exec(const char *cmd);

This runs a SQLite command directly. All results are printed to 
standard output (STDOUT).

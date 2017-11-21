/* 
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT license.
 */

#include <stdlib.h>
#include "dsp.h"
#include "sqlite3.h"

struct whisper_db {
    whisper_eyejam *ej;
    whisper_track *tracks;
    sqlite3 *db;
    int is_open;
};

static whisper_db the_db;
static int db_exec(whisper_db *db, const char *cmd);

static int callback(void *ud, int argc, char **argv, char **azColName)
{
    int i;
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int get_song_count(whisper_db *db)
{
    sqlite3_stmt *stmt;
    int count;

    sqlite3_prepare_v2(db->db,
    "SELECT COUNT(*) FROM SONGS;",
    -1,
    &stmt,
    NULL);

    sqlite3_step(stmt);

    count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count;
}

static void create_tables(whisper_db *db)
{
    db_exec(db,
    "CREATE TABLE IF NOT EXISTS CLIPS(ID INTEGER PRIMARY KEY,"
    "MAXVOICES INTEGER,"
    "NVOICES INTEGER,"
    "MAXLENGTH INTEGER,"
    "LENGTH INTEGER,"
    "DATA BLOB"
    ");"
    );

    db_exec(db,
    "CREATE TABLE IF NOT EXISTS TRACKS(ID INTEGER PRIMARY KEY,"
    "GAIN REAL,"
    "LOOPMODE INTEGER,"
    "CLIP_0 INTEGER,"
    "CLIP_1 INTEGER,"
    "CLIP_2 INTEGER,"
    "CLIP_3 INTEGER,"
    "CLIP_4 INTEGER"
    ");"
    );

    db_exec(db,
    "CREATE TABLE IF NOT EXISTS SONGS(ID INTEGER PRIMARY KEY,"
    "GAIN REAL,"
    "TEMPO REAL,"
    "TRACK_0 INTEGER,"
    "TRACK_1 INTEGER,"
    "TRACK_2 INTEGER,"
    "TRACK_3 INTEGER,"
    "TRACK_4 INTEGER"
    ");"
    );
    
    db_exec(db,
    "CREATE TABLE IF NOT EXISTS PRESETS(ID INTEGER PRIMARY KEY,"
    "TRINITY_0 INTEGER,"
    "TRINITY_1 INTEGER,"
    "SURGEON_0 INTEGER,"
    "SURGEON_1 INTEGER,"
    "DRUMKIT INTEGER"
    ");"
    );
}

static int db_open(whisper_db *db, const char *name)
{
    int rc;
    rc = sqlite3_open(name, &db->db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db->db));
        db->is_open = 0;
        sqlite3_close(db->db);
    } else {
        db->is_open = 1;
        create_tables(db);
    }
    return rc;
}

/* TODO: Someday, Eyejam may want to be refactored to rely on less global data */
static void db_eyejam_set(whisper_db *db, whisper_eyejam *eyejam)
{
    db->ej = eyejam;
}

EXPORT void whisper_eyejam_db_open(const char *name)
{
    db_open(&the_db, name);
    /* this doesn't do anything yet, but it may be a feature for later */
    db_eyejam_set(&the_db, NULL);
}

static void db_close(whisper_db *db)
{
    if(db->is_open) {
        sqlite3_close(db->db);
        db->is_open = 0;
    }
}

EXPORT void whisper_eyejam_db_close()
{
    db_close(&the_db);
}

static void do_nothing(void *nothing)
{

}

static int db_save_clip(whisper_db *db, whisper_clip *clip, int id, int *row_id)
{
    int maxvoices;
    int nvoices;
    int maxlength;
    int length;
    size_t size;
    unsigned char *data;
    int rc;

    sqlite3_stmt *stmt;

    if(!wclip_get_modflag(clip)) {
        *row_id = id;
        return 0;
    }

    if(!db->is_open) {
        fprintf(stderr, "db_save: db not open! aborting...\n");
        return 0;
    }

    /* get clip metadata */
    maxvoices = wclip_get_maxvoices(clip);
    nvoices = wclip_get_nvoices(clip);
    maxlength = wclip_get_maxlength(clip);
    length = wclip_get_length(clip);

    /* get clip data and size */
    data = wclip_get_data(clip);
    size = wclip_get_data_size(clip);
    

    /* prepare insert statement, assuming id is positive */
    if(id > 0) {
        sqlite3_prepare_v2(db->db, 
        /*
        "REPLACE INTO CLIPS(ID, MAXVOICES, NVOICES, MAXLENGTH, LENGTH, DATA)"
        "VALUES(?0, ?1, ?2, ?3, ?4, ?5);",
        */
        "REPLACE INTO CLIPS(ID, MAXVOICES, NVOICES, MAXLENGTH, LENGTH, DATA) VALUES(?1, ?2, ?3, ?4, ?5, ?6);",
        -1, 
        &stmt,
        NULL);
        sqlite3_bind_int(stmt, 1, id);
        *row_id = id;
    } else {
        sqlite3_prepare_v2(db->db, 
        "REPLACE INTO CLIPS(MAXVOICES, NVOICES, MAXLENGTH, LENGTH, DATA)"
        "VALUES(?2, ?3, ?4, ?5, ?6);",
        -1, 
        &stmt,
        NULL);
    }


    sqlite3_bind_int(stmt, 2, maxvoices);
    sqlite3_bind_int(stmt, 3, nvoices);
    sqlite3_bind_int(stmt, 4, maxlength);
    sqlite3_bind_int(stmt, 5, length);

    /*TODO: does this blob need to be freed here? */
    sqlite3_bind_blob(stmt, 6, data, size, do_nothing);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE) {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db->db));
    }

    sqlite3_finalize(stmt);

    /* if id is negative/zero, find the newly appended value and save it to 
     * row_id 
     */
    if(id < 1) {
       /* SELECT MAX(ID) FROM CLIPS; */
       sqlite3_prepare_v2(db->db, 
       "SELECT MAX(ID) FROM CLIPS;",
       -1,
       &stmt,
       NULL);
       sqlite3_step(stmt);
       *row_id = sqlite3_column_int(stmt, 0);
       fprintf(stderr, "new ID of %d created\n", *row_id);
    }

    fprintf(stderr, "Saved clip %d\n", *row_id);
    return rc;
}

EXPORT void whisper_eyejam_db_save_clip(int track_id, int clip_id, int id)
{
    int row_id;
    whisper_clip *clip;

    clip = whisper_clip_get(track_id, clip_id);
    db_save_clip(&the_db, clip, id, &row_id);
}

static int db_load_clip(whisper_db *db, int track, int clip_id, int id)
{
    whisper_clip *clip;
    int maxvoices;
    int nvoices;
    int maxlength;
    int length;
    size_t size;
    unsigned char *data;
    const unsigned char *blob;
    int bytes;
    int i;

    sqlite3_stmt *stmt;

    if(!db->is_open) {
        fprintf(stderr, "db_load: db not open! aborting...\n");
        return 0;
    }

    /* get clip from global data. */
    clip = whisper_clip_get(track, clip_id);

    /* get clip data and size */
    data = wclip_get_data(clip);
    size = wclip_get_data_size(clip);
    

    sqlite3_prepare_v2(db->db, 
    "SELECT * FROM CLIPS WHERE(ID == ?1);",
    -1, 
    &stmt,
    NULL);

    sqlite3_bind_int(stmt, 1, id);

    sqlite3_step(stmt);

/*TODO: check what the succesful return code is, as it is not SQLITE_DONE*/
/*
    if(rc != SQLITE_DONE) {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db->db));
        sqlite3_finalize(stmt);
        return 0;
    }
*/
    maxvoices = sqlite3_column_int(stmt, 1);
    nvoices = sqlite3_column_int(stmt, 2);
    maxlength = sqlite3_column_int(stmt, 3);
    length = sqlite3_column_int(stmt, 4);
    bytes = sqlite3_column_bytes(stmt, 5);
    blob = sqlite3_column_blob(stmt, 5);

    /* set variables */

    wclip_set_maxvoices(clip, maxvoices);
    wclip_set_nvoices(clip, nvoices);
    wclip_set_maxlength(clip, maxlength);
    wclip_set_length(clip, length);
    /* do a poor mans memcopy */

    if(bytes > size) bytes = size;

    for(i = 0; i < bytes; i++) {
        data[i] = blob[i];
    }

    sqlite3_finalize(stmt);
    return 1;
}

EXPORT void whisper_eyejam_db_load_clip(int track, int clip, int id)
{
    db_load_clip(&the_db, track, clip, id);
}

static int save_track(whisper_db *db, 
    int track_id, int id, int *row_id, int is_default)
{
    int clips[5]; /* Note: hardcoded to 5 here, not using WHISPER_NCLIPS */
    SPFLOAT gain;
    int loopmode;
    unsigned int c;
    whisper_track *track;
    whisper_clip *clip;
    int clip_row_id;
    int tmp_row_id;
    sqlite3_stmt *stmt;
    int rc;
    int clip_offset;

    track = whisper_tracks_get_track(track_id);

    if(!wtrack_is_modified(track)) {
        *row_id = id;
        return 0;
    }

    /* first, save track clips into clip table */
    if(is_default) {
        clip_offset = ((id - 1) * 5) + 1;
        for(c = 0; c < 5; c++) {
            clip = wtrack_get_clip(track, c);
            tmp_row_id = c + clip_offset;
            db_save_clip(db, clip, tmp_row_id, &clip_row_id); 
            /* save returned clip ID */
            clips[c] = clip_row_id;
        }
    } else {
        for(c = 0; c < 5; c++) {
            clip = wtrack_get_clip(track, c);
            tmp_row_id = wclip_get_row_id(clip);
            db_save_clip(db, clip, tmp_row_id, &clip_row_id); 
            /* save returned clip ID */
            clips[c] = clip_row_id;
        }
    }

    /* collect track information to be saved */

    gain = wtrack_get_gain(track);
    loopmode = wtrack_get_loopmode(track);
  
    /* create SQL query */
    
    /* prepare insert statement, assuming id is positive */
    if(id > 0) {
        sqlite3_prepare_v2(db->db, 
        "REPLACE INTO TRACKS"
        "(ID, GAIN, LOOPMODE, CLIP_0, CLIP_1, CLIP_2, CLIP_3, CLIP_4) "
        "VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);",
        -1, 
        &stmt,
        NULL);
        sqlite3_bind_int(stmt, 1, id);
        *row_id = id;
    } else {
        sqlite3_prepare_v2(db->db, 
        "REPLACE INTO TRACKS"
        "(ID, GAIN, LOOPMODE, CLIP_0, CLIP_1, CLIP_2, CLIP_3, CLIP_4) "
        "VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);",
        -1, 
        &stmt,
        NULL);
    }
    /* bind gain and loopmode */
    sqlite3_bind_double(stmt, 2, gain);
    sqlite3_bind_int(stmt, 3, loopmode);

    /* bind clip row_ids */
    for(c = 0; c < 5; c++) {
        sqlite3_bind_int(stmt, c + 4, clips[c]);
    }
    
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE) {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db->db));
    }

    sqlite3_finalize(stmt);

    /* if id is negative/zero, find the newly appended value and save it to 
     * row_id. 
     * TODO: refactor this since it appears in clips as well 
     */
    if(id < 1) {
       /* SELECT MAX(ID) FROM CLIPS; */
       sqlite3_prepare_v2(db->db, 
       "SELECT MAX(ID) FROM TRACKS;",
       -1,
       &stmt,
       NULL);
       sqlite3_step(stmt);
       *row_id = sqlite3_column_int(stmt, 0);
       fprintf(stderr, "track: new ID of %d created\n", *row_id);
    }

    fprintf(stderr, "---saved track %d\n", *row_id);
    return rc;
}

static int db_save_track(whisper_db *db, int track_id, int id, int *row_id)
{
    return save_track(db, track_id, id, row_id, 0);
}

static int db_save_track_default(whisper_db *db, int track_id, int id, int *row_id)
{
    return save_track(db, track_id, id, row_id, 1);
}

EXPORT void whisper_eyejam_db_save_track(int track, int id)
{
    int row_id;
    db_save_track(&the_db, track, id, &row_id);
}

static int db_load_track(whisper_db *db, int track_id, int id)
{
    int c;
    int clip_row_id;
    sqlite3_stmt *stmt;
    SPFLOAT gain;
    int loopmode;
    
    if(!db->is_open) {
        fprintf(stderr, "db_load_track: db is not open. aborting.\n");
        return 0;
    }

    sqlite3_prepare_v2(db->db, 
    "SELECT * FROM TRACKS WHERE(ID == ?1)",
    -1, 
    &stmt,
    NULL);
    sqlite3_bind_int(stmt, 1, id);

    sqlite3_step(stmt);
    /* get loopmode and gain */
    gain = sqlite3_column_double(stmt, 1);

    loopmode = sqlite3_column_int(stmt, 2);
    whisper_tracks_loopmode(track_id, loopmode);
    whisper_tracks_gain(track_id, gain);

    for(c = 0; c < 5; c++) {
        /* load clips */
        clip_row_id = sqlite3_column_int(stmt, c + 3);
        db_load_clip(db, track_id, c, clip_row_id);
    }

    return 0;
}

EXPORT void whisper_eyejam_db_load_track(int track, int id)
{
    db_load_track(&the_db, track, id);
}

static int db_save_presets(whisper_db *db, unsigned int song_id)
{
    sqlite3_stmt *stmt;
    int rc;
    sqlite3_prepare_v2(db->db, 
    "REPLACE INTO PRESETS"
    "(ID, TRINITY_0, TRINITY_1, SURGEON_0, SURGEON_1, DRUMKIT) "
    "VALUES(?1, ?2, ?3, ?4, ?5, ?6);",
    -1, 
    &stmt,
    NULL);
    sqlite3_bind_int(stmt, 1, song_id);
    sqlite3_bind_int(stmt, 2, whisper_trinity_preset_number(0));
    sqlite3_bind_int(stmt, 3, whisper_trinity_preset_number(1));
    sqlite3_bind_int(stmt, 4, whisper_surgeon_preset_number(0));
    sqlite3_bind_int(stmt, 5, whisper_surgeon_preset_number(1));
    sqlite3_bind_int(stmt, 6, -100);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE) {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db->db));
    }

    sqlite3_finalize(stmt);
    return 1;
}

static int db_save_song(whisper_db *db, unsigned int song_id)
{
    int t;
    int id;
    int row_id;
    int tracks[5]; /* Hardcoded at 5 tracks instead of WHISPER_NTRACKS */
    SPFLOAT gain;
    SPFLOAT tempo;
    sqlite3_stmt *stmt;
    int rc;

    if(!db->is_open) {
        fprintf(stderr, "db_save_song: db is not open. aborting.\n");
        return 0;
    }

    if(song_id < 1) {
       fprintf(stderr, "song id must be greater than 0!\n"); 
       return 0;
    }

    if(song_id == 1) {
        for(t = 0; t < 5; t++) {
            id = t + 1;
            db_save_track_default(db, t, id, &row_id);
            /* save track row_id for later use*/
            tracks[t] = row_id;
        }
    } else {
        for(t = 0; t < 5; t++) {
            id = whisper_tracks_get_row_id(t);
            db_save_track(db, t, id, &row_id);
            /* save track row_id for later use*/
            tracks[t] = row_id;
        }
    }

    /* get tempo and gain from global eyejam instance */
    tempo = whisper_eyejam_tempo_get();
    gain = whisper_eyejam_gain_get();

    /* set up SQL statement */
    sqlite3_prepare_v2(db->db, 
    "REPLACE INTO SONGS"
    "(ID, GAIN, TEMPO, TRACK_0, TRACK_1, TRACK_2, TRACK_3, TRACK_4) "
    "VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8);",
    -1, 
    &stmt,
    NULL);
    sqlite3_bind_int(stmt, 1, song_id);
    sqlite3_bind_double(stmt, 2, gain);
    sqlite3_bind_double(stmt, 3, tempo);

    for(t = 0; t < 5; t++) {
        sqlite3_bind_double(stmt, t + 4, tracks[t]);
    }
    
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE) {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db->db));
    }

    sqlite3_finalize(stmt);

    db_save_presets(db, song_id);
    
    return rc;
}

EXPORT void whisper_eyejam_db_save_song(int id)
{
    db_save_song(&the_db, id);
}

static int db_load_presets(whisper_db *db, unsigned int song_id)
{
    sqlite3_stmt *stmt;
    int tmp;
    sqlite3_prepare_v2(db->db, 
    "SELECT * FROM PRESETS WHERE(ID == ?1)",
    -1, 
    &stmt,
    NULL);
    sqlite3_bind_int(stmt, 1, song_id);
    sqlite3_step(stmt);

    tmp = sqlite3_column_int(stmt, 1);
    whisper_trinity_preset(0, tmp);
    tmp = sqlite3_column_int(stmt, 2);
    whisper_trinity_preset(1, tmp);
    sqlite3_finalize(stmt);
    return 1;
}

static int db_load_song(whisper_db *db, unsigned int song_id)
{
    int t;
    int track_id;
    sqlite3_stmt *stmt;
    SPFLOAT gain;
    SPFLOAT tempo;

    if(!db->is_open) {
        fprintf(stderr, "db_save_song: db is not open. aborting.\n");
        return 0;
    }

    if(song_id < 1) {
       fprintf(stderr, "song id must be greater than 0!\n"); 
       return 0;
    }

    
    sqlite3_prepare_v2(db->db, 
    "SELECT * FROM SONGS WHERE(ID == ?1)",
    -1, 
    &stmt,
    NULL);
        
    sqlite3_bind_int(stmt, 1, song_id);
    sqlite3_step(stmt);
/*
    if(rc != SQLITE_DONE) {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db->db));
    }
*/

    /* get tempo and gain */
    gain = sqlite3_column_double(stmt, 1);
    tempo = sqlite3_column_double(stmt, 2);

    whisper_eyejam_tempo_set(tempo);
    whisper_eyejam_gain(gain);

    for(t = 0; t < 5; t++) {
        /* load tracks */
        track_id = sqlite3_column_int(stmt, t + 3);
        db_load_track(db, t, track_id);
    }
    
    sqlite3_finalize(stmt);

    db_load_presets(db, song_id);

    return 0;
}

EXPORT void whisper_eyejam_db_load_song(int id)
{
    db_load_song(&the_db, id);
}

static int db_load_default(whisper_db *db)
{
/*
    int c, t;
    int id;
    if(!db->is_open) {
        fprintf(stderr, "db_save_default: db is not open. aborting.\n");
    }

    id = 1;
    for(t = 0; t < WHISPER_NTRACKS; t++) {
        for(c = 0; c < WHISPER_NCLIPS; c++) {
            db_load_clip(db, t, c, id);
            id++;
        }
    }

    return 0;
*/
    if(get_song_count(db) == 0) {
        whisper_eyejam_demo_clips();
    } else {
        db_load_song(db, 1);
    }
    return 0;
}

EXPORT void whisper_eyejam_db_load_default()
{
    db_load_default(&the_db);
}

static int db_save_default(whisper_db *db)
{
    db_save_song(db, 1);
    return 0;
}

EXPORT void whisper_eyejam_db_save_default()
{
    db_save_default(&the_db);
}

static int db_exec(whisper_db *db, const char *cmd)
{
    int rc;
    char *zErrMsg = 0;
    rc = sqlite3_exec(db->db, cmd, callback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return rc;
}

EXPORT void whisper_eyejam_db_exec(const char *cmd)
{
    db_exec(&the_db, cmd);
}

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* deezer-player.self get access to Deezer awesome media contents
 *
 * Copyright (C) 2013 Deezer
 *
 * Licensed under Deezer copyright
 *
 * @author Frederic Germain
 * @author Cyril Picheney
 *
 */

#ifndef DEEZER_PLAYER_H
#define DEEZER_PLAYER_H

#include <stdbool.h>
#include <stdint.h>
#include "deezer-connect.h"
#include "deezer-object.h"

typedef unsigned int dz_index_in_playlist;

/**
 * @defgroup DeezerCApiPlayer Deezer Player C API
 * @ingroup DeezerCApi
 * @brief Render the Deezer contents on behalf of Deezer subscribers.
 *
 * A valid session needs to be opened by the subscriber.\n
 * The player needs the authorization tokens and url to start streaming.
 *
 * @{
 */

#define DZ_INDEX_IN_PLAYLIST_INVALID  ((dz_index_in_playlist)-1) /**< Index in playlist has not been set yet, not a valid value. */
#define DZ_INDEX_IN_PLAYLIST_PREVIOUS ((dz_index_in_playlist)-2) /**< Play the previous track in the playlist. */
#define DZ_INDEX_IN_PLAYLIST_CURRENT  ((dz_index_in_playlist)-3) /**< Play the current track in the playlist. */
#define DZ_INDEX_IN_PLAYLIST_NEXT     ((dz_index_in_playlist)-4) /**< Play the next track in the playlist. */

/**
 * @brief Player events.
 *
 */
typedef enum {
    
    DZ_PLAYER_EVENT_UNKNOWN,                             /**< Player event has not been set yet, not a valid value. */
    
    // Data access related event.
    DZ_PLAYER_EVENT_LIMITATION_FORCED_PAUSE,             /**< Another deezer player session was created elsewhere, the player has entered pause mode. */

    // Track selection related event.
    DZ_PLAYER_EVENT_PLAYLIST_TRACK_NOT_AVAILABLE_OFFLINE,/**< You're offline, the track is not available. */
    DZ_PLAYER_EVENT_PLAYLIST_TRACK_NO_RIGHT,             /**< You don't have the right to render this track. */
    DZ_PLAYER_EVENT_PLAYLIST_TRACK_RIGHTS_AFTER_AUDIOADS,/**< You have right to play it, but you should render an ads first :
                                                              - Use dz_player_event_get_advertisement_infos_json().
                                                              - Play an ad with dz_player_play_audioads().
                                                              - Wait for #DZ_PLAYER_EVENT_RENDER_TRACK_END.
                                                              - Use dz_player_play() with previous track or DZ_PLAYER_PLAY_CMD_RESUMED_AFTER_ADS (to be done even on radios for now).
                                                          */
    DZ_PLAYER_EVENT_PLAYLIST_SKIP_NO_RIGHT,              /**< You're on a radio, and you had no right to do skip. */
    
    DZ_PLAYER_EVENT_PLAYLIST_TRACK_SELECTED,             /**< A track is selected among the ones available on the server, and will be fetched and read. */
    
    DZ_PLAYER_EVENT_PLAYLIST_NEED_NATURAL_NEXT,          /**< We need a new natural_next action. */
    
    // Data loading related event.
    DZ_PLAYER_EVENT_MEDIASTREAM_DATA_READY,              /**< Data is ready to be introduced into audio output (first data after a play). */
    DZ_PLAYER_EVENT_MEDIASTREAM_DATA_READY_AFTER_SEEK,   /**< Data is ready to be introduced into audio output (first data after a seek). */

    // Play (audio rendering on output) related event.
    DZ_PLAYER_EVENT_RENDER_TRACK_START_FAILURE,       /**< Error, track is unable to play. */
    DZ_PLAYER_EVENT_RENDER_TRACK_START,               /**< A track has started to play. */
    DZ_PLAYER_EVENT_RENDER_TRACK_END,                 /**< A track has stopped because the stream has ended. */
    DZ_PLAYER_EVENT_RENDER_TRACK_PAUSED,              /**< Currently on paused. */
    DZ_PLAYER_EVENT_RENDER_TRACK_SEEKING,             /**< Waiting for new data on seek. */
    DZ_PLAYER_EVENT_RENDER_TRACK_UNDERFLOW,           /**< Underflow happened whilst playing a track. */
    DZ_PLAYER_EVENT_RENDER_TRACK_RESUMED,             /**< Player resumed play after a underflow or a pause. */
    DZ_PLAYER_EVENT_RENDER_TRACK_REMOVED,             /**< Player stopped playing a track. */

} dz_player_event_t;

/**
 * @brief Player event handle declaration.\n
 * This handle is returned to the application through the dz_player_onevent_cb() callback.
 */
DECLARE_CLASS(dz_player_event);

/**
 * @brief Get the event type of a player event instance.
 * @param self A player event instance.
 *
 */
LIBDEEZER_API dz_player_event_t dz_player_event_get_type(dz_player_event_handle self);

/**
 * @brief Get the playlist context for a player event.
 *
 * For most player events, you can get the streaming mode #dz_streaming_mode_t and the index in the playlist.
 *
 * @param self               The player event that the context will be retrieved.
 * @param out_streaming_mode The streaming mode from which the event has occurred.
 * @param out_idx            The corresponding index in the playlist of the event.
 * @return true if the context has been returned, false otherwise.
 */
LIBDEEZER_API bool dz_player_event_get_playlist_context(dz_player_event_handle self,
                                                        dz_streaming_mode_t *out_streaming_mode,
                                                        dz_index_in_playlist *out_idx);

/**
 * @brief Get the Deezer API information for the player event instance.
 * @note Only available for #DZ_PLAYER_EVENT_PLAYLIST_TRACK_SELECTED events.
 * @param self A player event instance.
 *
 */
LIBDEEZER_API const char* dz_player_event_track_selected_dzapiinfo(dz_player_event_handle self);

/**
 * @brief Get the Deezer API information for the next track of a player event instance.
 * @note Only available for #DZ_PLAYER_EVENT_PLAYLIST_TRACK_SELECTED events.
 * @param self A player event instance.
 *
 */
LIBDEEZER_API const char* dz_player_event_track_selected_next_track_dzapiinfo(dz_player_event_handle self);

/**
 * @brief Get the playback rights for a track of a player event instance.
 * @note Only available for #DZ_PLAYER_EVENT_PLAYLIST_TRACK_SELECTED events.
 * @param self A player event instance.
 * @param out_can_pause_unpause Pause/unpause availability for the track.
 * @param out_can_seek          Seek availability for the track.
 * @param out_nb_skip_allowed   Number of skips available for the track.
 * @return true if the output information is valid.
 */
LIBDEEZER_API bool dz_player_event_track_selected_rights(dz_player_event_handle self,
                                                         bool* out_can_pause_unpause,
                                                         bool* out_can_seek,
                                                         int*  out_nb_skip_allowed);
/**
 * @brief Find out if the current track is a preview (30s).
 * 
 * @note Only available for #DZ_PLAYER_EVENT_PLAYLIST_TRACK_SELECTED events.
 * @param self A player event instance.
 * @return true if the selected track event is for a preview, false in all other cases.
 */
LIBDEEZER_API bool dz_player_event_track_selected_is_preview(dz_player_event_handle self);

/**
 * @brief Get information about displayed advertisements.
 *
 * @note Only available for #DZ_PLAYER_EVENT_PLAYLIST_TRACK_RIGHTS_AFTER_AUDIOADS events.
 * @param self A player event instance.
 */
LIBDEEZER_API const char* dz_player_event_get_advertisement_infos_json(dz_player_event_handle self);


/**
 * @brief Add an hint about why a new play is done.
 */
typedef enum {
    DZ_PLAYER_PLAY_CMD_UNKNOWN,           /**< Player command has not been set yet, not a valid value. */
    DZ_PLAYER_PLAY_CMD_START_TRACKLIST,   /**< A new tracklist was loaded and a track played. */
    DZ_PLAYER_PLAY_CMD_JUMP_IN_TRACKLIST, /**< The user jump into a new song in the current tracklist. */
    DZ_PLAYER_PLAY_CMD_NEXT,              /**< Next button. */
    DZ_PLAYER_PLAY_CMD_PREV,              /**< Prev button. */
    DZ_PLAYER_PLAY_CMD_DISLIKE,           /**< Dislike button. */
    DZ_PLAYER_PLAY_CMD_NATURAL_END,       /**< Natural end. */
    DZ_PLAYER_PLAY_CMD_RESUMED_AFTER_ADS, /**< Reload after playing an ads. */
} dz_player_play_command_t;

/**
 * @brief Player tracklist autoplay modes.
 *
 * Defines the behavior of the player when it finishes playing a track.
 */
typedef enum {

    DZ_TRACKLIST_AUTOPLAY_MODE_UNKNOWN, /**< Autoplay mode has not been set yet, not a valid value. */
    
    /** Do not auto-play a track (This mode is only available when playing single track tracklists) */
    DZ_TRACKLIST_AUTOPLAY_MANUAL,
    
    /** (Not available) */
    DZ_TRACKLIST_AUTOPLAY_MODE_ONE,
    /** (Not available) */
    DZ_TRACKLIST_AUTOPLAY_MODE_ONE_REPEAT,
    
    /** Used to play radios. Using this flag indicates the player to
     *  automatically play the next track in the tracklist.\n
     *  Radios are infinite playlists, tracks are added as soon
     *  as we get close to the end of the current tracklist.
     */
    DZ_TRACKLIST_AUTOPLAY_MODE_NEXT,
    /** (Not available) */
    DZ_TRACKLIST_AUTOPLAY_MODE_NEXT_REPEAT,
    
    /** (Not available) */
    DZ_TRACKLIST_AUTOPLAY_MODE_RANDOM,
    /** (Not available) */
    DZ_TRACKLIST_AUTOPLAY_MODE_RANDOM_REPEAT,

} dz_tracklist_autoplay_mode_t;


/**
 * @brief Handle to a single streaming session (one URL).
 */
DECLARE_CLASS(dz_player);


/**
 * @brief Prototype of the on metadata available callback.
 *
 * You have to assume the #dz_track_metadata_t object is invalid
 * after the callback returns,\n but you may retain it, then release it
 * by calling dz_object_retain() or dz_object_release().
 * @param handle             Deezer player handle.
 * @param metadata           The handle of the metadata information.
 * @param userdata           A reference to the user’s data.
 */
typedef void (*dz_player_onmetadata_cb)(
    dz_player_handle handle,
    dz_track_metadata_handle metadata,
    void* userdata
);

/**
 * @brief Prototype of the cache progress callback.
 * @param handle             Deezer player handle.
 * @param progress           The cache progress information in microseconds.
 * @param userdata           A reference to the user’s data.
 */
typedef void (*dz_player_onindexprogress_cb)(
    dz_player_handle handle,
    dz_useconds_t progress,
    void* userdata
);

/**
 * @brief Prototype of the render progress callback.
 * @param handle             Deezer player handle.
 * @param progress           The playback progress information in microseconds.
 * @param userdata           A reference to the user’s data.
 */
typedef void (*dz_player_onrenderprogress_cb)(
    dz_player_handle handle,
    dz_useconds_t progress,
    void* userdata
);

/**
 * @brief Prototype of the on event callback.
 * @param handle             Deezer player handle.
 * @param event              The event information.
 * @param userdata           A reference to the user’s data.
 */
typedef void (*dz_player_onevent_cb)(
    dz_player_handle handle,
    dz_player_event_handle event,
    void* userdata
);

/**
 * @brief Constructor for Deezer player.
 *
 * @param connect A deezer connect handle.
 * @returns A player handle.
 */
LIBDEEZER_API dz_player_handle dz_player_new(dz_connect_handle connect);



/**
 * @brief Register the event callback.
 *
 * Only one callback can be registered per player instance.
 * Should be done after dz_player_new() and before dz_player_load().
 *
 * @param self Deezer player handle
 * @param cb   The dz_player_onerror_cb() callback.
 */
LIBDEEZER_API dz_error_t dz_player_set_event_cb(dz_player_handle self,
                                                dz_player_onevent_cb cb);



/**
 * @brief Register a on data callback.
 *
 * Only one callback can be registered per player instance.
 * Should be done after dz_player_new() and before dz_player_load().
 *
 * @param self Deezer player handle.
 * @param cb   The dz_player_ondata_cb() callback.
 */
LIBDEEZER_API dz_error_t dz_player_set_metadata_cb(dz_player_handle self,
                                                   dz_player_onmetadata_cb cb);


/**
 * @brief Register a callback called when more data are available locally.
 *
 * Only one callback can be registered per player instance.
 * Should be done after dz_player_new() and before dz_player_load()
 *
 * @param self       Deezer player handle.
 * @param cb         The dz_player_onprogress_cb() callback.
 * @param refresh_us Approximate refresh time self callback is called.
 * @return DZ_ERROR_NO_ERROR if OK
 */
LIBDEEZER_API dz_error_t dz_player_set_index_progress_cb(dz_player_handle self,
                                                         dz_player_onindexprogress_cb cb,
                                                         dz_useconds_t refresh_us);

/**
 * @brief Register a callback that will be called when there is more audio
 * data rendered (played on an audio output).
 *
 * Only one callback can be registered per player instance.
 * Should be done after dz_player_new and before dz_player_load
 *
 * @param self       Deezer player handle.
 * @param cb         The dz_player_onprogress_cb() callback.
 * @param refresh_us Approximate refresh time self callback is called.
 * @return DZ_ERROR_NO_ERROR if OK
 */
LIBDEEZER_API dz_error_t dz_player_set_render_progress_cb(dz_player_handle self,
                                                          dz_player_onrenderprogress_cb cb,
                                                          dz_useconds_t refresh_us);


/**
 * @brief Activate the player.
 *
 * @note Activate the player after the callback has been set.
 *
 * @param self       Deezer player handle you want to activate
 * @param supervisor Application context that will be returned when events occur.
 */
LIBDEEZER_API dz_error_t dz_player_activate(dz_player_handle self, void*supervisor);


/**
 * @brief Deactivate the player.
 *
 * @param self               Deezer player handle you want to deactivate.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_player_deactivate(dz_player_handle self,
                                              dz_activity_operation_callback cb,
                                              void* operation_userdata);

/**
 * @brief Set the track quality.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param quality            Audio track quality.
 */
LIBDEEZER_API dz_error_t dz_player_set_track_quality(dz_player_handle self,
                                                     dz_activity_operation_callback cb,
                                                     void* operation_userdata,
                                                     dz_track_quality_t quality);

/**
 * @brief Get track quality.
 *
 * Get the currently configured track quality.
 *
 * @param self               Deezer player handle.
 * @return The audio track quality.
 */
LIBDEEZER_API dz_track_quality_t dz_player_get_track_quality(dz_player_handle self);


/**
 * @brief Set the tracks/radio to play.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param tracklist_data: 
 *        - On demand: a deezer url, such as like dzmedia:///track/[track_id]
 *        - (Not available) On demand: a json with a list of deezer url
 *        - (Not available) On demand: an url to an offline or online dz album or playlist
 *        - radio: a theme/artist/user/playlist radio dzradio:///[radio_id]
 *          radio_id is like "radio-???", "artist-???", "user-???", "playlist-???"
 */
LIBDEEZER_API dz_error_t dz_player_load(dz_player_handle self,
                                        dz_activity_operation_callback cb,
                                        void* operation_userdata,
                                        const char *tracklist_data);

/**
 * @brief Set the radio to play by type and id.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param radio_type         "radio", "artist", "user" or "playlist".
 * @param radio_id           A string representing the integer id of the radio id.
 */
LIBDEEZER_API dz_error_t dz_player_load_radio(dz_player_handle self,
                                              dz_activity_operation_callback cb,
                                              void* operation_userdata,
                                              const char *radio_type,
                                              const char *radio_id);


/**
 * @brief Cache a track for playback purposes.
 *
 * Use this function to prefetch the next track to be played.\n
 * This function can be called multiple times if need be (i.e. each time the next track changes).
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param track_url          A track url, like dzmedia:///track/[track_id]
 */
LIBDEEZER_API dz_error_t dz_player_cache_next(dz_player_handle self,
                                              dz_activity_operation_callback cb,
                                              void* operation_userdata,
                                              const char *track_url);

/**
 * @brief Begin playing the already loaded tracklist.
 *        The player gets data and renders it.
 *
 * The player can be used in several ways:
 * * Albums & Playlists: SDK does not support playing albums and playlists directly,\n
 *   instead, you need to play them track by track.
 *   To do this, use the #DZ_TRACKLIST_AUTOPLAY_MANUAL mode and pass-in the track ids.
 * * Radios: to play a radio, use the #DZ_TRACKLIST_AUTOPLAY_MODE_NEXT mode so that next tracks\n
 *   are launched automatically.
 *
 * @note idx is only supported for radios, possible values are #DZ_INDEX_IN_PLAYLIST_CURRENT or #DZ_INDEX_IN_PLAYLIST_NEXT.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param command            See function comments.
 * @param mode               Autoplay mode.
 * @param idx                Index to play.
 */
LIBDEEZER_API dz_error_t dz_player_play(dz_player_handle self,
                                        dz_activity_operation_callback cb,
                                        void* operation_userdata,
                                        dz_player_play_command_t command,
                                        dz_tracklist_autoplay_mode_t mode,
                                        dz_index_in_playlist idx);

/**
 * @brief Play an audio ad when it is requested.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_player_play_audioads(dz_player_handle self,
                                                 dz_activity_operation_callback cb,
                                                 void* operation_userdata);

/**
 * @brief Put the player in stop state.
 *
 * Cached buffers have been dropped, and the data callback has stopped being triggered by the player.
 * The player can start streaming again from the begin if you call dz_player_play() again.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_player_stop(dz_player_handle self,
                                        dz_activity_operation_callback cb,
                                        void* operation_userdata);

/**
 * @brief Put the player on pause.
 *
 * No cache buffer is dropped, and the data callback stops being triggered by the player.
 * @note You need to call dz_player_resume() to start data callback again.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_player_pause(dz_player_handle self,
                                         dz_activity_operation_callback cb,
                                         void* operation_userdata);

/**
 * @brief Resumes playback after a pause.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_player_resume(dz_player_handle self,
                                           dz_activity_operation_callback cb,
                                           void* operation_userdata);

/**
 * @brief Seek at the specified time position.
 *
 * Cached buffers have been dropped, and the player will prepare data
 * from the closest position.
 *
 * If the player is in stop state, it goes into the pause state.
 * Otherwise the player stays in play / pause state.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data..
 * @param pos_usecond        Position in microseconds from the begin of track.
 */
LIBDEEZER_API dz_error_t dz_player_seek(dz_player_handle self,
                                        dz_activity_operation_callback cb,
                                        void* operation_userdata,
                                        dz_useconds_t pos_usecond);


/**
 * @brief Define the player volume percentage as an integer. Its value can be from 0 to 100.
 *
 */
typedef int dz_player_volume_percent_t;


/**
 * @brief Set the player output volume.
 *
 * Changing this volume won't impact the volume of the other applications
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param volume             Volume to set in percent (0 to 100).
 */
LIBDEEZER_API dz_error_t dz_player_set_output_volume(dz_player_handle self,
                                                     dz_activity_operation_callback cb,
                                                     void* operation_userdata,
                                                     dz_player_volume_percent_t volume);


/**
 * @brief Mute the stream.
 *
 * Cached buffers have been dropped, and the player will prepare data
 * from the closest position.
 *
 * If the player was in stop state, it goes into the pause state.
 * Otherwise the player keeps its play/pause state.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param muted              Mute value.
 */
LIBDEEZER_API dz_error_t dz_player_set_output_mute(dz_player_handle self,
                                                   dz_activity_operation_callback cb,
                                                   void* operation_userdata,
                                                   bool muted);

/**
 * @brief Setup things to be able to do cross fadings.
 *
 * "things" are :
 * - Event #DZ_PLAYER_EVENT_PLAYLIST_NEED_NATURAL_NEXT fade_duration_ms before the end of current track.
 * - Cross fading between two tracks when a natural_next happens.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param fade_duration_ms   Duration in microseconds of cross-fading (0 to remove, typically less than 10s).
 */
LIBDEEZER_API dz_error_t dz_player_set_crossfading_duration(dz_player_handle self,
                                                            dz_activity_operation_callback cb,
                                                            void* operation_userdata,
                                                            dz_mseconds_t fade_duration_ms);

/**
 * @brief Set the UI context.
 *
 * Used by deezer for statistical purposes to improve client experience.
 *
 * Example of ui_app_context:
 * * Feed/hear this:
 *   - flow: {“t":"feed_user_radio", “id":USER_ID_VALUE}
 *   - card_album: {“t":"feed_album", “id":ALBUM_ID_VALUE,"c":CARD_ID_VALUE}
 *   - card_playlist: {“t":"feed_playlist", “id":PLAYLIST_VALUE,"c":CARD_ID_VALUE}
 *   - card_track: {“t":"feed_playlist", “id":TRACK_ID_VALUE,"c":CARD_ID_VALUE}
 *   - card_smartradio: {“t":"feed_smartradio", “id":ARTIST_ID_VALUE,"c":CARD_ID_VALUE}
 * * Search:
 *   - track: {“t":"search_page", “id":QUERY}
 * * Playlist pages:
 *   - playlist: {“t":"playlist_page", “id":PLAYLIST_VALUE}
 *   - loved tracks: {“t":"loved_page", “id":PLAYLIST_VALUE}
 * * Album page:
 *   - album: {“t":"album_page", “id":ALBUM_ID_VALUE}
 *
 *
 * Example of ui_app_context:
 * - track_page:[trackid]
 * - album_page:[albumid]
 * - playlist_page:[playlistid]
 * - downloads_page
 * - personalsong_page
 * - folder_page
 * - radio_page
 * - smartradio_page
 * - search_page
 * - collection_track
 * - collection_playlist
 * - collection_album
 * - collection_radio
 * - artist_top
 * - artist_discography
 * - artist_radio
 * - artist_exclu
 * - artist_playlist
 * - album_discography
 * - tops_album
 * - tops_track
 * - recommendations_playlist
 * - recommendations_release
 * - recommendations_lastfm
 * - recommendations_album
 * - recommendations_friend_top
 * - recommendations_friend_share_track
 * - recommendations_friend_share_album
 * - recommendations_friend_share_playlist
 * - ticker_track
 * - ticker_playlist
 * - ticker_album
 * - profile_top
 * - profile_album
 * - profile_playlist
 * - selection_billboard
 * - selection_album
 * - selection_playlist
 * - selection_top
 * - selection_radio
 * - player_default_playlist
 * - contextmenu_playlist
 * - contextmenu_album
 * - facebook_track
 * - facebook_artist
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param sz_json            A JSON string representing the context.
 */
LIBDEEZER_API dz_error_t dz_player_set_log_context(dz_player_handle self,
                                                   dz_activity_operation_callback cb,
                                                   void* operation_userdata,
                                                   const char* sz_json);
/**
 * @brief Set the UI context.
 *
 * Used by deezer for statistics purpose to improve client experience.
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param sz_id              Application context ID   as described for dz_player_set_log_context().
 * @param sz_type            Application context Type as described for dz_player_set_log_context().
 * @param sz_card            Application context Card as described for dz_player_set_log_context().
 */
LIBDEEZER_API dz_error_t dz_player_set_log_context3(dz_player_handle self,
                                                    dz_activity_operation_callback cb,
                                                    void* operation_userdata,
                                                    const char* sz_id,
                                                    const char* sz_type,
                                                    const char* sz_card);
/**
 * @brief Define the renderer event handle that will be used by dz_player_onrendererevent_cb() callback to notify renderer event.
 *
 */
DECLARE_CLASS(dz_renderer_event);

/**
 * @brief Get the information for the renderer event.
 * @param self The event from which the information will be extracted.
 *
 */
LIBDEEZER_API const char* dz_renderer_event_get_infos(dz_renderer_event_handle self);

/**
 * @brief Prototype of the on renderer event callback
 *
 * @param self     Deezer player handle.
 * @param event    The handle of the renderer event that has just occured.
 * @param userdata A reference to the user’s data.
 */
typedef void (*dz_player_onrendererevent_cb)(
dz_player_handle handle,
dz_renderer_event_handle event,
void* userdata
);

/**
 * @brief Register a on renderer event callback.
 *
 * Only one callback can be registered per player instance.
 * Should be done after dz_player_new() and before dz_player_load().
 *
 * @param self Deezer player handle.
 * @param cb   The dz_player_onerror_cb() callback.
 */
LIBDEEZER_API dz_error_t dz_player_set_renderer_event_cb(dz_player_handle self,
                                                         dz_player_onrendererevent_cb cb);

/**
 * @brief Define a renderer state handle that will be used when notifying a renderer state change event.
 *
 */
DECLARE_CLASS(dz_renderer_state);

/**
 * @brief Get information about a renderer state handle.
 *
 * @param self A renderer state handle that the infos need to be extracted.
 * @return A JSON string describing the state of the renderer.
 */
LIBDEEZER_API const char* dz_renderer_state_get_infos(dz_renderer_state_handle self);

/**
 * @brief Get the current state of all renderers.
 *
 * Asynchronously return a dz_renderer_state_handle object.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_player_renderer_get_state(dz_player_handle self,
                                                      dz_activity_operation_callback cb,
                                                      void* operation_userdata);

/**
 * @brief Change/add/remove a renderer to the playback.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param json_list_of_renderer_id JSON list the renderer IDs like "[\"id1\",\"id2\"]".
 */
LIBDEEZER_API dz_error_t dz_player_renderer_change_selection(dz_player_handle self,
                                                             dz_activity_operation_callback cb,
                                                             void* operation_userdata,
                                                             const char *json_list_of_renderer_id);

/**
 * @brief Change/add/remove a renderer to the playback.
 *
 * @param self               Deezer player handle.
 * @param cb                 The dz_activity_operation_callback() callback called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param renderer_id        The renderer id impacted.
 * @param volume             The volume to be applied to the renderer.
 */
LIBDEEZER_API dz_error_t dz_player_renderer_set_volume(dz_player_handle self,
                                                       dz_activity_operation_callback cb,
                                                       void* operation_userdata,
                                                       const char *renderer_id,
                                                       dz_player_volume_percent_t volume);

/** @} */ //End of group tag (for documentation purpose)

#endif

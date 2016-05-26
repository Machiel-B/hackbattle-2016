/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* deezer-offline
 *
 * Copyright (C) 2013 Deezer
 *
 * Licensed under Deezer copyright
 *
 * @author Frederic Germain
 * @author Cyril Picheney
 * @date   30/01/2014
 *
 */


#ifndef libdeezer_deezer_offline_h
#define libdeezer_deezer_offline_h

#include "deezer-connect.h"

/**
 * @defgroup DeezerCApiOfflineApi Deezer Offline C API
 * @ingroup DeezerCApi
 * @brief Synchronize user data with server and enable offline mode.
 * For more information on how the Native SDK stores data resources offline,
 * take a look at the [local storage](@ref DeezerLocalstorageModel) page.
 *   
 *
 * @{
 */


/**
 * @brief Connect offline states.
 */
typedef enum {
    DZ_OFFLINE_SYNC_STATE_UNKNOWN,             /**< Sync state has not been set yet, not a valid value. */
    DZ_OFFLINE_SYNC_STATE_UNSYNCED,            /**< Resource is not available locally. */
    DZ_OFFLINE_SYNC_STATE_SYNCED,              /**< Resource and its dependencies are available. */
    DZ_OFFLINE_SYNC_STATE_SYNCING,             /**< Resource is being synced. */
    DZ_OFFLINE_SYNC_STATE_ENQUEUED_FOR_SYNC,   /**< Resource will be synced ASAP. */
    DZ_OFFLINE_SYNC_STATE_UNSYNCING,           /**< Resource is being unsynced. */
    DZ_OFFLINE_SYNC_STATE_ENQUEUED_FOR_UNSYNC, /**< Resource will be unsynced ASAP. */
    DZ_OFFLINE_SYNC_STATE_SYNC_ERROR,          /**< There was an error during a sync ---> solution: relaunch sync automatically. */
} dz_offline_sync_state_t;

/**
 * @brief Connect offline events.
 *
 */
typedef enum {
    DZ_OFFLINE_EVENT_UNKNOWN,           /**< Offline event has not been set yet, not a valid value. */
    DZ_OFFLINE_EVENT_NEW_SYNC_STATE,    /**< Send when the state on a resource has changed. */
    DZ_OFFLINE_EVENT_DOWNLOAD_PROGRESS, /**< Send progress event on a resource operation. */
} dz_offline_event_t;

/** 
 * @brief Event object from offline.
 *
 */
DECLARE_CLASS(dz_offline_event);

/**
 * @brief Get the offline event type.
 *
 * @param self Offline event.
 * @return The type of event.
 */
LIBDEEZER_API dz_offline_event_t dz_offline_event_get_type(dz_offline_event_handle self);

/**
 * @brief Get the resource type of a resource associated with
 * an offline event.
 *
 * @note Return value could be "dz_tracklist", "dz_track_offline" ...
 *
 * @param self Offline event.
 * @return the UTF8 string of the [resource](@ref DeezerLocalstorageModel) type
 */
LIBDEEZER_API const char* dz_offline_event_get_resource_type(dz_offline_event_handle self);

/**
 * @brief Get the resource id of a resource associated with
 * an offline event.
 *
 * @param self Offline event.
 * @return The UTF8 string of the [resource](@ref DeezerLocalstorageModel) id.
 */
LIBDEEZER_API const char* dz_offline_event_get_resource_id(dz_offline_event_handle self);

/**
 * @brief Get the resource version of a resource associated with
 * an offline event.
 *
 * @param self Offline event.
 * @return The UTF8 string of the [resource](@ref DeezerLocalstorageModel) id.
 */
LIBDEEZER_API const char* dz_offline_event_get_resource_version(dz_offline_event_handle self);


/**
 * @brief Get the resource type of the container resource associated with
 * an offline event.
 *
 * @note The container resource is the resource originally responsible for the sync, usually
 * a tracklist.
 *
 * return value could be "dz_tracklist", ...
 *
 * @param self Offline event.
 * @return The UTF8 string of the [resource](@ref DeezerLocalstorageModel) type
 */
LIBDEEZER_API const char* dz_offline_event_get_container_resource_type(dz_offline_event_handle self);

/**
 * @brief Get the resource id of the container resource associated with
 * an offline event.
 *
 * @note The container resource is the resource originally responsible for the sync, usually
 * a tracklist.
 *
 * return value could be "dz_tracklist", ...
 *
 * @param self Offline event.
 * @return The UTF8 string of the [resource](@ref DeezerLocalstorageModel) type
 */
LIBDEEZER_API const char* dz_offline_event_get_container_resource_id(dz_offline_event_handle self);

/**
 * @brief Get the resource version of the container resource associated with
 * an offline event.
 *
 * @note The container resource is the resource originally responsible for the sync, usually
 * a tracklist.
 *
 * return value could be "dz_tracklist", ...
 *
 * @param self Offline event.
 * @return The UTF8 string of the [resource](@ref DeezerLocalstorageModel) type
 */
LIBDEEZER_API const char* dz_offline_event_get_container_resource_version(dz_offline_event_handle self);


/**
 * @brief Get the track list id of a track list resource associated with
 * an offline event.
 *
 * @param self Offline event.
 * @return The UTF8 string of track list id.
 */
LIBDEEZER_API const char* dz_offline_event_tracklist_id(dz_offline_event_handle self);

/**
 * @brief Get the track id of an offline track resource associated with
 * an offline event.
 *
 * @param self Offline event.
 * @return The UTF8 string of the track id.
 */
LIBDEEZER_API dz_track_id_t dz_offline_event_trackoffline_trackid(dz_offline_event_handle self);

/**
 * @brief Get the track id of a track file resource associated with
 * an offline event.
 *
 * @param self Offline event.
 * @return The UTF8 string of the track id.
 */
LIBDEEZER_API dz_track_id_t dz_offline_event_trackfile_trackid(dz_offline_event_handle self);

/**
 * @brief Get the resource sync state.
 *
 * @param self Offline event of type DZ_OFFLINE_EVENT_NEW_SYNC_STATE.
 * @return The new state for the resource (cf. #dz_offline_sync_state_t).
 */
LIBDEEZER_API dz_offline_sync_state_t dz_offline_event_resource_get_sync_state(dz_offline_event_handle self);

/**
 * @brief State on a readable string.
 *
 * @param state_type State type.
 * @return A string corresponding to sate type.
 */
LIBDEEZER_API const char* dz_offline_state_to_cchar(dz_offline_sync_state_t state_type);

/**
 * @brief Get the currently downloaded resource size.
 *
 * @param self Offline event of type DZ_OFFLINE_EVENT_DOWNLOAD_PROGRESS.
 * @return The size in bytes.
 */
LIBDEEZER_API dz_bigsize_t dz_offline_event_download_progress_downloaded_size(dz_offline_event_handle self);

/**
 * @brief Get the resource size when completed.
 *
 * @param self Offline event of type DZ_OFFLINE_EVENT_DOWNLOAD_PROGRESS.
 * @return The size in bytes.
 */
LIBDEEZER_API dz_bigsize_t dz_offline_event_download_progress_complete_size(dz_offline_event_handle self);

/**
 * @brief Prototype for offline event callback.
 *
 */
typedef void (*dz_offline_on_event_cb)(
    dz_connect_handle handle,
    dz_offline_event_handle event,
    void* delegate
);

/**
 * @brief Set the offline event callback.
 *
 * This callback can be used to retrieve the download progress of a track as well as if it’s synchronized or not (aka the sync state).
 *
 * @note Set to NULL to disable.
 *
 * @param self Deezer connect handle.
 * @param cb   Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param offlineevent_cb Callback call on event.
 */
LIBDEEZER_API dz_error_t dz_offline_eventcb_set(dz_connect_handle self,
                                                dz_activity_operation_callback cb,
                                                void* operation_userdata,
                                                dz_offline_on_event_cb offlineevent_cb);
/**
 * @brief Set track quality of synced tracks.
 *
 * Set the encoding quality configured as DZ_TRACK_QUALITY_HIGHQUALITY at init.
 *
 * @note This parameter only impacts tracks that will be downloaded.
 * Previously downloaded tracks remain in the same format.
 *
 * @param self               Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param quality            The audio quality
 */
LIBDEEZER_API dz_error_t dz_offline_set_track_quality(dz_connect_handle self,
                                                     dz_activity_operation_callback cb,
                                                     void* operation_userdata,
                                                     dz_track_quality_t quality);

/**
 * @brief Add a resource to offline storage.
 *
 * The path tells the offline storage what kind of object you intend to save.
 * The offline storage manages the retrieval of the resource and its dependencies.
 *
 * To save a track list on file system, proceed as following:
 *
 *     dz_offline_synchronize(....,
 *                                        "/dzlocal/tracklist/playlist/554287425",
 *                                        "checksum82AF24AD29",
 *                                        "{ tracks: [ "3135553", "3135554" ] }");
 *
 * You can use any kind of track list, but the SDK was designed to synchronize albums
 * and playlists from Deezer servers.
 *
 * @param self               Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param sz_path            The path pointing to the location where you want to save the resource.
 * @param sz_version         The resource's version. The local storage engine synchronizes the lastest version for a given path.
 * @param sz_json_data       The data serialized as a UTF8 JSON char *.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_synchronize(dz_connect_handle self,
                                               dz_activity_operation_callback cb,
                                               void* operation_userdata,
                                               const char* sz_path,
                                               const char* sz_version,
                                               const char* sz_json_data);


/**
 * @brief Remove the offline mode from a resource.
 *
 * Example:
 *
 *     dz_offline_remove(...., "/dzlocal/tracklist/playlist/554287425");
 *
 * @param self               Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param sz_path            The resource path you intend to remove the offline mode.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_remove(dz_connect_handle self,
                                           dz_activity_operation_callback cb,
                                           void* operation_userdata,
                                           const char* sz_path);

/**
 * @brief Get the resource's synchronization state given by the path.
 *
 * The result is available via the async callback:
 *
 *     dz_offline_get_state(connect, my_on_get_sync_state_callback, "/dzlocal/tracklist/playlist/554287425");
 *
 *     void my_on_get_sync_state_callback(.... error, result) {
 *        if (error == DZ_ERROR_NO_ERROR) {
 *           const char* sz_json_state = dz_offline_state_to_json(result);
 *           // calls to asynchrously refresh your UI
 *        } else {
 *           // error, consider that the object is not synced
 *           // calls to asynchrously refresh your UI
 *        }
 *
 * @param self               Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param sz_path            The path of the resource you intend to get the state.
 * @param detailed           If true, get the state of each track file node in the playlist.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_get_state(dz_connect_handle self,
                                              dz_activity_operation_callback cb,
                                              void* operation_userdata,
                                              const char* sz_path,
                                              bool detailed);

/**
 * @brief Get the JSON representation of a sync state.
 *
 * @param sync_state A sync state object pointer.
 * @return A JSON pointer or NULL.
 */
LIBDEEZER_API const char* dz_offline_state_to_json(dz_object_handle sync_state);


/**
 * @brief Get the resource's state of synchronization given by the path.
 *
 * The result is available via the async callback:
 *
 *     dz_offline_get_state_batch(connect, my_on_get_sync_state_callback, "[ "/dzlocal/tracklist/playlist/554287425", "..." ]" ...);
 *
 *     void my_on_get_sync_state_callback(.... error, result) {
 *        if (error == DZ_ERROR_NO_ERROR) {
 *           const char* sz_json_state = dz_offline_state_batch_to_json(result);
 *           // calls to asynchrously refresh your UI
 *        } else {
 *           // error, consider that the object is not synced
 *           // calls to asynchrously refresh your UI
 *        }
 *
 * @param self               Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param sz_list_of_tracklist_path Resource path you intend to save.
 * @param detailed           If true, also get the state of each track node in the playlists.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_get_state_batch(dz_connect_handle self,
                                                   dz_activity_operation_callback cb,
                                                   void* operation_userdata,
                                                   const char* sz_list_of_tracklist_path,
                                                   bool detailed);

/**
 * @brief Get the JSON representation of a sync state.
 *
 * @param sync_state A sync state object pointer.
 * @return A JSON or NULL.
 */
LIBDEEZER_API const char* dz_offline_state_batch_to_json(dz_object_handle sync_state);


/**
 * @brief Get the currently available synced data for a resource.
 *
 * The result is available via the async callback (cf #dz_offline_get_state).
 *
 * This function can be used to get :
 * - the list of synced tracklists "/dzlib/tracklists"
 *
 * @param self               Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param sz_path            Resource path you want the data.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_get(dz_connect_handle self,
                                        dz_activity_operation_callback cb,
                                        void* operation_userdata,
                                        const char* sz_path);


/**
 * @brief Get the UTF8 c-string of the object retrieved by dz_offline_get().
 *
 * @param obj An object returned by dz_offline_get().
 * @return c char.
 */
LIBDEEZER_API const char* dz_offline_resource_to_json(dz_object_handle obj);

/** @} */ //End of group tag (for documentation purpose)

#endif

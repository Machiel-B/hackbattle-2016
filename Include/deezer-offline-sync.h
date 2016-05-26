/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* deezer-offline-sync
 *
 * Copyright (C) 2013 Deezer
 *
 * Licensed under Deezer copyright
 *
 * @author Frederic Germain
 * @author Cyril Picheney
 * @date   10/07/2014
 *
 */


#ifndef libdeezer_deezer_offline_sync_h
#define libdeezer_deezer_offline_sync_h


#include "deezer-connect.h"
#include "deezer-offline.h"

/**
 * @defgroup DeezerCApiOfflineSyncApi Deezer Offline C synchronous API
 * @ingroup DeezerCApi
 * @brief Synchronous version of the deezer-offline.h APIs.
 *
 * @note Asynchronous APIs are more efficient and non-blocking, if you can integrate them in your own API.
 *
 * @{
 */

/**
 * @brief Synchronous version of dz_offline_get_state() function.
 *
 * @note If (p_result != NULL), then p_result needs to be released with DZ_OBJECT_RELEASE
 *       when it is no longer needed.
 *
 * @param self                      Deezer connect handle.
 * @param sz_path                   The path of the resource you intend to get the state.
 * @param detailed                  If true, get the state of each track in the playlist.
 * @param p_result                  Out pointer to a resource object.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_get_state_sync(dz_connect_handle self,
                                                   const char* sz_path,
                                                   bool detailed,
                                                   dz_object_handle* p_result);


/**
 * @brief Synchronous version of dz_offline_get_state_batch() function.
 *
 * @note If (p_result != NULL), then p_result needs to be released with DZ_OBJECT_RELEASE
 *       when it is no longer needed.
 *
 * @param self                      Deezer connect handle.
 * @param sz_list_of_tracklist_path Resource path you intend to save.
 * @param detailed                  If true, get the state of each track in the playlist.
 * @param p_result                  Out pointer to a resource object.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_get_state_batch_sync(dz_connect_handle self,
                                                        const char* sz_list_of_tracklist_path,
                                                        bool detailed,
                                                        dz_object_handle* p_result);
/**
 * @brief Synchronous version of dz_offline_get() function.
 *
 * @note If (p_result != NULL), then p_result needs to be released with DZ_OBJECT_RELEASE
 *       when it is no longer needed.
 *
 * @param self                      Deezer connect handle.
 * @param sz_path                   Resource path you want the data.
 * @param p_result                  Out pointer to a resource object.
 * @return DZ_ERROR_NO_ERROR
 */
LIBDEEZER_API dz_error_t dz_offline_get_sync(dz_connect_handle self,
                                             const char* sz_path,
                                             dz_object_handle* p_result);

/** @} */


#endif

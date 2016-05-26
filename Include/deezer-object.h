/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* deezer-object
 *
 * Copyright (C) 2013 Deezer
 *
 * Licensed under Deezer copyright
 *
 * @author Frederic Germain
 * @author Cyril Picheney
 * @date   26/08/13
 *
 */

#ifndef libdeezer_dz_object_h
#define libdeezer_dz_object_h

#include <stdint.h>
#ifdef _WIN32
#ifndef bool
#include <stdbool.h>
#endif
#else
#include <stdbool.h>
#endif

#include <stddef.h>
#include <sys/types.h> /* off_t */

#ifdef _WIN32
#ifndef __cplusplus
#ifndef inline
#define inline _inline
#endif
#endif
#endif

#ifndef EXTERN_INLINE
# if __GNUC__ && !__GNUC_STDC_INLINE__
#  define EXTERN_INLINE extern inline
# else
#  define EXTERN_INLINE inline
# endif
#endif

#ifndef INLINE
#  define INLINE static inline
#endif


#ifdef _WIN32
#ifdef LIBDEEZER_EXPORTS
#define LIBDEEZER_API __declspec(dllexport)
#else
#define LIBDEEZER_API __declspec(dllimport)
#endif
#else
#define LIBDEEZER_API __attribute__((visibility("default")))
#endif

/** @brief Errors that can be returned by Deezer SDK API.
 */
typedef enum {
    DZ_ERROR_NO_ERROR          = 0x00000000,
    DZ_ERROR_NO_ERROR_ASYNC    = 0x00000001,
    DZ_ERROR_ERROR_ARG         = 0x00000002,
    DZ_ERROR_ERROR_STATE       = 0x00000003,
    DZ_ERROR_NOT_IMPLEMENTED   = 0x00000004,
    DZ_ERROR_ASYNC_CANCELED    = 0x00000005,
    
    DZ_ERROR_NOT_ENOUGH_MEMORY,
    DZ_ERROR_OS_ERROR,
    DZ_ERROR_UNSUPPORTED,
    DZ_ERROR_CLASS_NOT_FOUND,
    DZ_ERROR_JSON_PARSING,
    DZ_ERROR_XML_PARSING,
    DZ_ERROR_PARSING,
    DZ_ERROR_CLASS_INSTANTIATION,
    DZ_ERROR_RUNNABLE_ALREADY_STARTED,
    DZ_ERROR_RUNNABLE_NOT_STARTED,
    DZ_ERROR_CACHE_RESOURCE_OPEN_FAILED,
    DZ_ERROR_FS_FULL,
    DZ_ERROR_FILE_EXISTS,
    DZ_ERROR_IO_ERROR,

    DZ_ERROR_CATEGORY_CONNECT = 0x00010000,
    DZ_ERROR_CONNECT_SESSION_LOGIN_FAILED,
    DZ_ERROR_USER_PROFILE_PERM_DENIED,
    DZ_ERROR_CACHE_DIRECTORY_PERM_DENIED,
    DZ_ERROR_CONNECT_SESSION_NOT_ONLINE,
    DZ_ERROR_CONNECT_SESSION_OFFLINE_MODE,
    DZ_ERROR_CONNECT_NO_OFFLINE_CACHE,

    DZ_ERROR_CATEGORY_PLAYER  = 0x00020000,
    DZ_ERROR_PLAYER_PLAYLIST_NONE_SET,
    DZ_ERROR_PLAYER_PLAYLIST_BAD_INDEX,
    DZ_ERROR_PLAYER_PLAYLIST_NO_MEDIA,         /**< when trying to access non existing track/radio */
    DZ_ERROR_PLAYER_PLAYLIST_NO_RIGHTS,        /**< when trying to access track/radio with no rights */
    DZ_ERROR_PLAYER_PLAYLIST_RIGHT_TIMEOUT,    /**< when timoeout trying to get rights */
    DZ_ERROR_PLAYER_PLAYLIST_RADIO_TOO_MANY_SKIP,
    DZ_ERROR_PLAYER_PLAYLIST_NO_MORE_TRACK,
    DZ_ERROR_PLAYER_PAUSE_NOT_STARTED,
    DZ_ERROR_PLAYER_PAUSE_ALREADY_PAUSED,
    DZ_ERROR_PLAYER_UNPAUSE_NOT_STARTED,
    DZ_ERROR_PLAYER_UNPAUSE_NOT_PAUSED,
    DZ_ERROR_PLAYER_SEEK_NOT_SEEKABLE_NOT_STARTED,
    DZ_ERROR_PLAYER_SEEK_NOT_SEEKABLE_NO_DURATION,
    DZ_ERROR_PLAYER_SEEK_NOT_SEEKABLE_NOT_INDEXED,
    DZ_ERROR_PLAYER_SEEK_NOT_SEEKABLE,

    DZ_ERROR_CATEGORY_MEDIASTREAMER  = 0x00030000,
    DZ_ERROR_MEDIASTREAMER_BAD_URL_SCHEME,
    DZ_ERROR_MEDIASTREAMER_BAD_URL_HOST,
    DZ_ERROR_MEDIASTREAMER_BAD_URL_TRACK,
    DZ_ERROR_MEDIASTREAMER_NOT_AVAILABLE_OFFLINE,
    DZ_ERROR_MEDIASTREAMER_NOT_READABLE,
    DZ_ERROR_MEDIASTREAMER_NO_DURATION,
    DZ_ERROR_MEDIASTREAMER_NOT_INDEXED,
    DZ_ERROR_MEDIASTREAMER_SEEK_NOT_SEEKABLE,
    DZ_ERROR_MEDIASTREAMER_NO_DATA,
    DZ_ERROR_MEDIASTREAMER_END_OF_STREAM,
    DZ_ERROR_MEDIASTREAMER_ALREADY_MAPPED,
    DZ_ERROR_MEDIASTREAMER_NOT_MAPPED,
    
    DZ_ERROR_CATEGORY_OFFLINE  = 0x00040000,
    DZ_ERROR_OFFLINE_FS_FULL,
    
    DZ_ERROR_PLAYER_BAD_URL,
} dz_error_t;


#define DECLARE_CLASS(class_name) \
struct class_name; \
typedef struct class_name *class_name##_handle \

#ifdef __cplusplus
extern "C" {
#endif
 
DECLARE_CLASS(dz_object);

/** 
 * @brief Keep object reference and avoid autorelease.
 * @param obj The object handle that need to be retained.
 * @return the object handle that is retained.
 */
LIBDEEZER_API dz_object_handle dz_object_retain(dz_object_handle obj);

/**
 * @brief Release object reference.
 * @param obj The object handle that need to be released.
 */
LIBDEEZER_API void dz_object_release(dz_object_handle obj);

/**
 * @brief Keep object reference and avoid autorelease macro.
 */
#define DZ_OBJECT_RETAIN(obj) dz_object_retain((dz_object_handle)obj)

/**
 * @brief Release object reference macro.
 */
#define DZ_OBJECT_RELEASE(obj) dz_object_release((dz_object_handle)obj)
    
DECLARE_CLASS(dz_activity);

/**
 * @brief Prototype for any asynchronous operations of Deezer API.
 * @param delegate           Application context data pointer that provides back when the async result will be available.
 * @param operation_userdata A reference to the user’s data.
 * @param status             error code returned.
 * @param result the result of the operation
 */
typedef void (*dz_activity_operation_callback)(
    void* delegate,
    void* operation_userdata,
    dz_error_t status,
    dz_object_handle result);


/**
 * @brief Deactivate the application activity.
 * This function is the last one to be called after the player and connect have been deactivated.
 *
 * @param self               Application context A reference to the user’s data.
 * @param cb                 Callback that will be called when the application activity has been deactivated.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_activity_app_deactivate(dz_activity_handle self,
                                                    dz_activity_operation_callback cb,
                                                    void* operation_userdata);

/**
 * @brief Dump some information about bad state of lib.
 *
 * Calling this function could dump some interesting information for debug.
 *
 * @note Due to the way the Native SDK was designed, it means we didn't get the
 * notifications all tasks are finished.
 * This is an internal bug, and there is a leak, it should be fixed.
 *
 * @param self deezer connect handle.
 */
LIBDEEZER_API void dz_activity_debug_do_post_deactivation_analysis(dz_activity_handle self);
    
typedef uint32_t dz_mseconds_t;
typedef uint64_t dz_useconds_t;
typedef uint64_t dz_nseconds_t;

typedef unsigned long dz_size_t;
typedef uint32_t    dz_sizekB_t;
typedef uint64_t    dz_bigsize_t;

#define DZ_BIGSIZE_UNKNOWN ((dz_bigsize_t)-1)

#ifdef __cplusplus
}
#endif

#endif

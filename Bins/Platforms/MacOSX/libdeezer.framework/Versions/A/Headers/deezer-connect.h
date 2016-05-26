/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* deezer-connect.self a Deezer subscriber session
 *
 * Copyright (C) 2013 Deezer
 *
 * Licensed under Deezer copyright
 *
 * @author Frederic Germain
 * @author Cyril Picheney
 */
#ifndef DEEZER_CONNECT_H
#define DEEZER_CONNECT_H

#include "deezer-object.h"
#include "deezer-track.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * @defgroup DeezerCApiConnect Deezer Connect C API
 * @ingroup DeezerCApi
 * @brief API representing a Deezer session.
 * @{
 */
    
/**
 * @brief Streaming modes.
 *
 */
typedef enum {
    DZ_STREAMING_MODE_UNKNOWN,  /**< Mode is not known, not a valid value. */
    DZ_STREAMING_MODE_ONDEMAND, /**< On demand streaming mode. */
    DZ_STREAMING_MODE_RADIO,    /**< Radio streaming mode. */
} dz_streaming_mode_t;

    
/**
 * @brief Connect events.
 *
 */
typedef enum {
    DZ_CONNECT_EVENT_UNKNOWN,                           /**< Connect event has not been set yet, not a valid value. */
    DZ_CONNECT_EVENT_USER_OFFLINE_AVAILABLE,            /**< User logged in, and credentials from offline store are loaded. */
    
    DZ_CONNECT_EVENT_USER_ACCESS_TOKEN_OK,              /**< (Not available) dz_connect_login_with_email() ok, and access_token is available */
    DZ_CONNECT_EVENT_USER_ACCESS_TOKEN_FAILED,          /**< (Not available) dz_connect_login_with_email() failed */

    DZ_CONNECT_EVENT_USER_LOGIN_OK,                     /**< Login with access_token ok, infos from user available. */
    DZ_CONNECT_EVENT_USER_LOGIN_FAIL_NETWORK_ERROR,     /**< Login with access_token failed because of network condition. */
    DZ_CONNECT_EVENT_USER_LOGIN_FAIL_BAD_CREDENTIALS,   /**< Login with access_token failed because of bad credentials. */
    DZ_CONNECT_EVENT_USER_LOGIN_FAIL_USER_INFO,         /**< Login with access_token failed because of other problem. */
    DZ_CONNECT_EVENT_USER_LOGIN_FAIL_OFFLINE_MODE,      /**< Login with access_token failed because we are in forced offline mode. */
    
    DZ_CONNECT_EVENT_USER_NEW_OPTIONS,                  /**< User options have just changed. */
    
    DZ_CONNECT_EVENT_ADVERTISEMENT_START,               /**< A new advertisement needs to be displayed. */
    DZ_CONNECT_EVENT_ADVERTISEMENT_STOP,                /**< An advertisement needs to be stopped. */

} dz_connect_event_t;

/**
 * @brief Connect event handle returned by a #dz_connect_onevent_cb() callback.
 *
 */
DECLARE_CLASS(dz_connect_event);

/**
 * @brief Get the type of the event.
 *
 */
LIBDEEZER_API dz_connect_event_t dz_connect_event_get_type(dz_connect_event_handle self);

/**
 * @brief Get the access token of the current session for a #DZ_CONNECT_EVENT_USER_ACCESS_TOKEN_OK event.
 *
 */
LIBDEEZER_API const char* dz_connect_event_get_access_token(dz_connect_event_handle self);

/**
 * @brief Get a JSON describing events.
 *
 * Available for events:
 * - #DZ_CONNECT_EVENT_ADVERTISEMENT_START
 * - #DZ_CONNECT_EVENT_ADVERTISEMENT_STOP
 *
 */
LIBDEEZER_API const char* dz_connect_event_get_advertisement_infos_json(dz_connect_event_handle self);

/**
 * @brief Get api /user/me JSON.
 *
 * Available for events:
 * - #DZ_CONNECT_EVENT_USER_OFFLINE_AVAILABLE
 * - #DZ_CONNECT_EVENT_USER_LOGIN_OK
 */
LIBDEEZER_API const char* dz_connect_event_get_user_me_json(dz_connect_event_handle self);
    
/**
 * @brief Get api /options JSON.
 *
 * Available for events:
 * - #DZ_CONNECT_EVENT_USER_OFFLINE_AVAILABLE
 * - #DZ_CONNECT_EVENT_USER_LOGIN_OK
 * - #DZ_CONNECT_EVENT_USER_NEW_OPTIONS
 */
LIBDEEZER_API const char* dz_connect_event_get_options_json(dz_connect_event_handle self);

/**
 * @brief Connect smartcache events.
 *
 */
typedef enum {
    DZ_CONNECT_CACHE_EVENT_UNKNOWN,                     /**< Cache event has not been set yet, not a valid value. */
    DZ_CONNECT_CACHE_EVENT_SMART_CACHE_SIZE_CHANGED,    /**< Smart cache size has changed */
    DZ_CONNECT_CACHE_EVENT_OFFLINE_CACHE_SIZE_CHANGED,  /**< Offline cache size has changed */
    DZ_CONNECT_CACHE_EVENT_PARTITION_NEARLY_FULL,       /**< Partition where the tracks are cached is almost full, only 200MB left */
    DZ_CONNECT_CACHE_EVENT_PARTITION_FULL,              /**< Partition where the tracks are cached is full */
} dz_connect_cache_event_t;

/**
 * @brief Connect cache event handle returned by a #dz_connect_on_cache_event_cb() callback.
 *
 */
DECLARE_CLASS(dz_connect_cache_event);

/**
 * @brief Get the type of the event.
 *
 */
LIBDEEZER_API dz_connect_cache_event_t dz_connect_cache_event_get_type(dz_connect_cache_event_handle self);

/**
 * @brief Get the event's size info.
 *
 * Available for events:
 * - #DZ_CONNECT_CACHE_EVENT_SMART_CACHE_SIZE_CHANGED.
 * - #DZ_CONNECT_CACHE_EVENT_OFFLINE_CACHE_SIZE_CHANGED.
 */
LIBDEEZER_API dz_sizekB_t dz_connect_cache_event_get_new_size(dz_connect_cache_event_handle self);

/**
 * @brief Handle to a connection session.
 */
DECLARE_CLASS(dz_connect);

/**
 * @brief Prototype for on connect event callback.
 */
typedef void (*dz_connect_onevent_cb)(
    dz_connect_handle handle,
    dz_connect_event_handle event,
    void* delegate
);


/**
 * @brief Prototype for crash report notification delegate.
 * @return true If the application has previously crashed (in the previous session).
 */
typedef bool (*dz_connect_crash_reporting_delegate)(void);
    
/**
 * @brief Prototype for smartcache usage callback.
 *
 */
typedef void (*dz_connect_on_cache_event_cb)(
    dz_connect_handle             handle,
    dz_connect_cache_event_handle event,
    void*                         delegate
    );

    
/**
 * @brief Configuration for a Deezer connect handle.
 */
struct dz_connect_configuration {
    /** The Deezer application ID being used. */
    const char* app_id;
    
    /** Product ID of the application being used.\n
     *  In a compact form, only ascii characters and '.' (point character) are allowed.
     */
    const char* product_id;

    /** Build product ID which will be used by the Native SDK. */
    const char* product_build_id;
    
    /** User profile path of application being used. */
    const char* user_profile_path;
    
    /** (Optional) #connect_event_cb() connect event callback of application being used. */
    dz_connect_onevent_cb connect_event_cb;
    
    /** (Optional) Use to allow discovery. */
    const char* anonymous_blob;
    
    /** (Optional) Delegate used to let the lib know if the application has previously crashed.\n
      * If this delegate is not set, an internal crash reporter will be used by the Native SDK.
      */
    dz_connect_crash_reporting_delegate app_has_crashed_delegate;
};

/**
 * @brief Constructor for dz_connect_handle.
 *
 * @param config see #dz_connect_configuration.
 * @returns      A Deezer connect handle.
 */
LIBDEEZER_API dz_connect_handle dz_connect_new(const struct dz_connect_configuration* config);


/**
 * @brief Disable log generated by the lib.
 *
 * @param self A Deezer connect handle.
 * @returns    DZ_ERROR_NO_ERROR.
 */
LIBDEEZER_API dz_error_t dz_connect_debug_log_disable(dz_connect_handle self);
    
/**
 * @brief Get the device_id associated with the lib.
 *
 * @param self A Deezer connect handle.
 * @returns    A string to a device_id.
 */
LIBDEEZER_API const char* dz_connect_get_device_id(dz_connect_handle self);
    
/**
 * @brief Activate the connect session.
 *
 *
 * @param self     The Deezer connect handle you want to activate.
 * @param userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_connect_activate(dz_connect_handle self,
                                             void *userdata);


/**
 * @brief Deactivate for dz_connect_handle.
 *
 * @param self               The Deezer connect handle you want to deactivate.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_connect_deactivate(dz_connect_handle self,
        dz_activity_operation_callback cb,
        void* operation_userdata);

/**
 * @brief Set current OAuth access token.
 *
 * @param self               A Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param access_token       OAuth access token.
 */
LIBDEEZER_API dz_error_t dz_connect_set_access_token(dz_connect_handle self,
                                                     dz_activity_operation_callback cb,
                                                     void*       operation_userdata,
                                                     const char* access_token);

/**
 * @brief Logout the user.
 *
 * User data is kept, and removed only if another user logs in after.
 *
 * @param self               A Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 */
LIBDEEZER_API dz_error_t dz_connect_logout(dz_connect_handle self,
                                           dz_activity_operation_callback cb,
                                           void* operation_userdata);

/**
 * @brief Set the path of the Native SDK cache.
 *
 * @note The Native SDK has a local cache mechanism which can be erased at any time.\n
 * In such a case, the cache will be updated over again whenever it has internet access.
 *
 * @param self               A Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param local_path         Path to the cache.
 */
LIBDEEZER_API dz_error_t dz_connect_cache_path_set(dz_connect_handle self,
                                                   dz_activity_operation_callback cb,
                                                   void* operation_userdata,
                                                   const char* local_path);

/**
 * @brief Set the audio smartcache quota.
 *
 * The smartcache allows the player to store tracks while playing.
 *
 * A smartcache should not be shared between application or several instances of your
 * application.
 *
 * @param self               A Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param quota_kB           Quota in kB.
 */
LIBDEEZER_API dz_error_t dz_connect_smartcache_quota_set(dz_connect_handle self,
        dz_activity_operation_callback cb,
        void* operation_userdata,
        dz_sizekB_t quota_kB);

/**
 * @brief Set the smartcache event callback.
 *
 * @note Set to NULL to disable.
 *
 * @param self               A Deezer connect handle.
 * @param cb                 Function called when the async result is available.
 * @param operation_userdata A reference to the user’s data.
 * @param cacheevent_cb      Callback call on event.
 */
LIBDEEZER_API dz_error_t dz_connect_cache_eventcb_set(dz_connect_handle self,
                                                      dz_activity_operation_callback cb,
                                                      void* operation_userdata,
                                                      dz_connect_on_cache_event_cb cacheevent_cb);
    
/**
  * @brief Get the cache size (smartcache + offline tracks).
  *
  * @param self Deezer connect handle.
  * @return     The current size in bytes.
  */
LIBDEEZER_API dz_bigsize_t dz_connect_cache_current_size(dz_connect_handle self);
    
/**
  * @brief Force offline mode in lib.
  *
  * @param self               A Deezer connect handle.
  * @param cb                 Function called when the async result is available.
  * @param operation_userdata A reference to the user’s data.
  */
LIBDEEZER_API dz_error_t dz_connect_cache_flush(dz_connect_handle self,
                                                dz_activity_operation_callback cb,
                                                void* operation_userdata);

/**
 * @brief Force offline mode in lib.
 *
 * @param self                A Deezer connect handle.
 * @param cb                  Function called when the async result is available.
 * @param operation_userdata  A reference to the user’s data.
 * @param offline_mode_forced is offline mode enforced.
 */
LIBDEEZER_API dz_error_t dz_connect_offline_mode(dz_connect_handle self,
                                                 dz_activity_operation_callback cb,
                                                 void* operation_userdata,
                                                 bool  offline_mode_forced);

/**
 * @brief Send an app event.
 *
 * There is no return code.
 *
 * Example of event_name:
 * - menu_click > type (settings, feed, explore), session_duration.
 *
 * @param self          A Deezer connect handle.
 * @param sz_event_name Event name string.
 * @param sz_properties Serialized JSON of properties.
 * @param sz_path       Path to associate event with.
 */
LIBDEEZER_API dz_error_t dz_connect_push_app_event(dz_connect_handle self,
                                                   const char* sz_event_name,
                                                   const char* sz_properties,
                                                   const char* sz_path);

/**
 * @brief Get the build id of the Native SDK.
 *
 * @return A string representing the build id of the Native SDK.
 */
LIBDEEZER_API char* dz_connect_get_build_id(void);

/** @} */ //End of group tag (for documentation purpose)

#ifdef __cplusplus
}
#endif

#endif

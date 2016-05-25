#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
static __inline int sleep(int s) {
	Sleep(1000*s);
	return 0;
}
#else
#include <unistd.h>
#endif

#include <deezer-connect.h>
#include <deezer-player.h>

#define log(fmt, ...) printf("[%s:%d]" fmt, __FILE__, __LINE__, ##__VA_ARGS__);

static int print_version   = true;
static int print_device_id = true;

static int nb_track_to_play = 1; // This number includes the audio ads to play
static int nb_track_played = 0;

#define USER_ACCESS_TOKEN        "frNnhmDqYdzmtXu6qOIDKPJnK415WcuIE301ooDUz7V161ZmeU" // Sample access token corresponding to a free user account, to be replaced by yours.
#define YOUR_APPLICATION_ID      "180542"     // SET YOUR APPLICATION ID
#define YOUR_APPLICATION_NAME    "alfred" // SET YOUR APPLICATION NAME
#define YOUR_APPLICATION_VERSION "00001"      // SET YOUR APPLICATION VERSION
#define YOUR_APPLICATION_SECRET  "7932091700cff6970c95965fa9a15521"
#ifdef _WIN32
#define USER_CACHE_PATH          "c:\\dzr\\dzrcache_NDK_SAMPLE" // SET THE USER CACHE PATH, This pasth must already exist
#else
#define USER_CACHE_PATH          "/var/tmp/dzrcache_NDK_SAMPLE" // SET THE USER CACHE PATH, This pasth must already exist
#endif
    
static dz_connect_handle dzconnect;
static dz_player_handle  dzplayer;
static int activation_count   = 0;
char sz_track_url[256];

static void app_shutdown();

static void dz_connect_on_deactivate(void* delegate,
                                     void* operation_userdata,
                                     dz_error_t status,
                                     dz_object_handle result);
static void dz_player_on_deactivate(void* delegate,
                                    void* operation_userdata,
                                    dz_error_t status,
                                    dz_object_handle result);

static void app_launch_play();

// callback for dzconnect events
static void app_connect_onevent_cb(dz_connect_handle handle,
                                   dz_connect_event_handle event,
                                   void* delegate);
// callback for dzplayer events
static void app_player_onevent_cb(dz_player_handle handle,
                                  dz_player_event_handle event,
                                  void* supervisor);


int main(int argc, char* argv[]) {

    struct dz_connect_configuration config;
    dz_error_t dzerr = DZ_ERROR_NO_ERROR;

    if (print_version) {
        log("<-- Deezer native SDK Version : %s\n", dz_connect_get_build_id());
    }
    strcpy(sz_track_url, "dzmedia:///track/");
    strcat(sz_track_url, argv[1]);
    memset(&config, 0, sizeof(struct dz_connect_configuration));

    config.app_id            = YOUR_APPLICATION_ID;      // SET YOUR APPLICATION ID
    config.product_id        = YOUR_APPLICATION_NAME;    // SET YOUR APPLICATION NAME
    config.product_build_id  = YOUR_APPLICATION_VERSION; // SET YOUR APPLICATION VERSION
    config.user_profile_path = USER_CACHE_PATH;          // SET THE USER CACHE PATH
    config.connect_event_cb  = app_connect_onevent_cb;

    log("--> Application ID:    %s\n", config.app_id);
    log("--> Product ID:        %s\n", config.product_id);
    log("--> Product BUILD ID:  %s\n", config.product_build_id);
    log("--> User Profile Path: %s\n", config.user_profile_path);

    dzconnect = dz_connect_new(&config);

    if (dzconnect == NULL) {
        log("dzconnect null\n");
        return -1;
    }

    if (print_device_id) {
        log("Device ID: %s\n", dz_connect_get_device_id(dzconnect));
    }

    dzerr = dz_connect_debug_log_disable(dzconnect);
    if (dzerr != DZ_ERROR_NO_ERROR) {
        log("dz_connect_debug_log_disable error\n");
        return -1;
    }

    dzerr = dz_connect_activate(dzconnect, NULL);
    if (dzerr != DZ_ERROR_NO_ERROR) {
        log("dz_connect_activate error\n");
        return -1;
    }
    activation_count++;

    /* Calling dz_connect_cache_path_set()
     * is mandatory in order to have the attended behavior */
    dz_connect_cache_path_set(dzconnect, NULL, NULL, USER_CACHE_PATH);

    dzplayer = dz_player_new(dzconnect);
    if (dzplayer == NULL) {
        log("dzplayer null\n");
        return -1;
    }

    dzerr = dz_player_activate(dzplayer, NULL);
    if (dzerr != DZ_ERROR_NO_ERROR) {
        log("dz_player_activate error\n");
        return -1;
    }
    activation_count++;

    dzerr = dz_player_set_event_cb(dzplayer, app_player_onevent_cb);
    if (dzerr != DZ_ERROR_NO_ERROR) {
        log("dz_player_set_event_cb error\n");
        return -1;
    }

    dzerr = dz_connect_set_access_token(dzconnect,NULL, NULL, USER_ACCESS_TOKEN);
    if (dzerr != DZ_ERROR_NO_ERROR) {
        log("dz_connect_set_access_token error\n");
        return -1;
    }

    /* Calling dz_connect_offline_mode(FALSE) is mandatory to force the login */
    dzerr = dz_connect_offline_mode(dzconnect, NULL, NULL, false);
    if (dzerr != DZ_ERROR_NO_ERROR) {
        log("dz_connect_offline_mode error\n");
        return -1;
    }

    while (1) {
        //log("- - - - wait until end of actions (c=%d)\n",activation_count);
        sleep (1);
        if (activation_count == 0) // exited normally
            break;
    }

    if (dzconnect) {
        dz_object_release((dz_object_handle)dzconnect);
        dzconnect = NULL;
    }
    if (dzplayer) {
        dz_object_release((dz_object_handle)dzplayer);
        dzplayer = NULL;
    }

    log("-- shutdowned --\n");
    return 0;
}

static void app_shutdown() {
    log("SHUTDOWN (1/2) - dzplayer = %p\n",dzplayer);
    if (dzplayer)
        dz_player_deactivate(dzplayer, dz_player_on_deactivate, NULL);
    log("SHUTDOWN (2/2) - dzconnect = %p\n",dzconnect);
    if (dzconnect)
        dz_connect_deactivate(dzconnect, dz_connect_on_deactivate, NULL);
}

void app_connect_onevent_cb(dz_connect_handle handle,
                            dz_connect_event_handle event,
                            void* delegate) {

    dz_connect_event_t type = dz_connect_event_get_type(event);
    switch (type) {
        case DZ_CONNECT_EVENT_USER_OFFLINE_AVAILABLE:
            log("++++ CONNECT_EVENT ++++ USER_OFFLINE_AVAILABLE\n");
            break;

        case DZ_CONNECT_EVENT_USER_ACCESS_TOKEN_OK:
            {
                const char* szAccessToken;
                szAccessToken = dz_connect_event_get_access_token(event);
                log("++++ CONNECT_EVENT ++++ USER_ACCESS_TOKEN_OK Access_token : %s\n", szAccessToken);
            }
            break;

        case DZ_CONNECT_EVENT_USER_ACCESS_TOKEN_FAILED:
            log("++++ CONNECT_EVENT ++++ USER_ACCESS_TOKEN_FAILED\n");
            break;

        case DZ_CONNECT_EVENT_USER_LOGIN_OK:
            log("++++ CONNECT_EVENT ++++ USER_LOGIN_OK\n");
            app_launch_play();
            break;

        case DZ_CONNECT_EVENT_USER_NEW_OPTIONS:
            log("++++ CONNECT_EVENT ++++ USER_NEW_OPTIONS\n");
            break;

        case DZ_CONNECT_EVENT_USER_LOGIN_FAIL_NETWORK_ERROR:
            log("++++ CONNECT_EVENT ++++ USER_LOGIN_FAIL_NETWORK_ERROR\n");
            break;

        case DZ_CONNECT_EVENT_USER_LOGIN_FAIL_BAD_CREDENTIALS:
            log("++++ CONNECT_EVENT ++++ USER_LOGIN_FAIL_BAD_CREDENTIALS\n");
            break;

        case DZ_CONNECT_EVENT_USER_LOGIN_FAIL_USER_INFO:
            log("++++ CONNECT_EVENT ++++ USER_LOGIN_FAIL_USER_INFO\n");
            break;

        case DZ_CONNECT_EVENT_USER_LOGIN_FAIL_OFFLINE_MODE:
            log("++++ CONNECT_EVENT ++++ USER_LOGIN_FAIL_OFFLINE_MODE\n");
            break;

        case DZ_CONNECT_EVENT_ADVERTISEMENT_START:
            log("++++ CONNECT_EVENT ++++ ADVERTISEMENT_START\n");
            break;

        case DZ_CONNECT_EVENT_ADVERTISEMENT_STOP:
            log("++++ CONNECT_EVENT ++++ ADVERTISEMENT_STOP\n");
            break;

        case DZ_CONNECT_EVENT_UNKNOWN:
        default:
            log("++++ CONNECT_EVENT ++++ UNKNOWN or default (type = %d)\n",type);
            break;
    }
}

static void app_launch_play() {
    log("LOAD & PLAY => %s\n", sz_track_url);
    dz_player_load(dzplayer, NULL, NULL, sz_track_url);
    dz_player_play(dzplayer, NULL, NULL,
                   DZ_PLAYER_PLAY_CMD_START_TRACKLIST,
                   DZ_TRACKLIST_AUTOPLAY_MANUAL,
                   0);
}

void app_player_onevent_cb( dz_player_handle       handle,
                            dz_player_event_handle event,
                            void *                 supervisor)
{
    dz_streaming_mode_t  streaming_mode;
    dz_index_in_playlist idx;

    dz_player_event_t type = dz_player_event_get_type(event);
    
    if (!dz_player_event_get_playlist_context(event, &streaming_mode, &idx)) {
        streaming_mode = DZ_STREAMING_MODE_ONDEMAND;
        idx = -1;
    }

    switch (type) {
    case DZ_PLAYER_EVENT_LIMITATION_FORCED_PAUSE:
        log("==== PLAYER_EVENT ==== LIMITATION_FORCED_PAUSE for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_PLAYLIST_TRACK_NO_RIGHT:
        log("==== PLAYER_EVENT ==== PLAYLIST_TRACK_NO_RIGHT for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_PLAYLIST_NEED_NATURAL_NEXT:
        log("==== PLAYER_EVENT ==== PLAYLIST_NEED_NATURAL_NEXT for idx: %d\n", idx);
        app_launch_play();
        break;

    case DZ_PLAYER_EVENT_PLAYLIST_TRACK_NOT_AVAILABLE_OFFLINE:
        log("==== PLAYER_EVENT ==== PLAYLIST_TRACK_NOT_AVAILABLE_OFFLINE for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_PLAYLIST_TRACK_RIGHTS_AFTER_AUDIOADS:
        log("==== PLAYER_EVENT ==== PLAYLIST_TRACK_RIGHTS_AFTER_AUDIOADS for idx: %d\n", idx);
        dz_player_play_audioads(dzplayer, NULL, NULL);
        break;

    case DZ_PLAYER_EVENT_PLAYLIST_SKIP_NO_RIGHT:
        log("==== PLAYER_EVENT ==== PLAYLIST_SKIP_NO_RIGHT for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_PLAYLIST_TRACK_SELECTED:
        {
            bool is_preview;
            bool can_pause_unpause;
            bool can_seek;
            int  nb_skip_allowed;
            const char *selected_dzapiinfo;
            const char *next_dzapiinfo;

            is_preview = dz_player_event_track_selected_is_preview(event);

            dz_player_event_track_selected_rights(event, &can_pause_unpause, &can_seek, &nb_skip_allowed);

            selected_dzapiinfo = dz_player_event_track_selected_dzapiinfo(event);
            next_dzapiinfo = dz_player_event_track_selected_next_track_dzapiinfo(event);

            log("==== PLAYER_EVENT ==== PLAYLIST_TRACK_SELECTED for idx: %d - is_preview:%d\n", idx, is_preview);
            log("\tcan_pause_unpause:%d can_seek:%d nb_skip_allowed:%d\n", can_pause_unpause, can_seek, nb_skip_allowed);
            if (selected_dzapiinfo)
            log("\tnow:%s\n", selected_dzapiinfo);
            if (next_dzapiinfo)
            log("\tnext:%s\n", next_dzapiinfo);
         }
        nb_track_played++;
        break;

    case DZ_PLAYER_EVENT_MEDIASTREAM_DATA_READY:
        log("==== PLAYER_EVENT ==== MEDIASTREAM_DATA_READY for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_MEDIASTREAM_DATA_READY_AFTER_SEEK:
        log("==== PLAYER_EVENT ==== MEDIASTREAM_DATA_READY_AFTER_SEEK for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_START_FAILURE:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_START_FAILURE for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_START:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_START for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_END:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_END for idx: %d\n", idx);
        log("\tnb_track_to_play : %d\tnb_track_played : %d\n",nb_track_to_play,nb_track_played);
        if (nb_track_to_play != -1 &&  // unlimited
            nb_track_to_play == nb_track_played) {
            app_shutdown();
        } else {
            app_launch_play();
        }
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_PAUSED:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_PAUSED for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_UNDERFLOW:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_UNDERFLOW for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_RESUMED:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_RESUMED for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_SEEKING:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_SEEKING for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_RENDER_TRACK_REMOVED:
        log("==== PLAYER_EVENT ==== RENDER_TRACK_REMOVED for idx: %d\n", idx);
        break;

    case DZ_PLAYER_EVENT_UNKNOWN:
    default:
        log("==== PLAYER_EVENT ==== UNKNOWN or default (type = %d)\n",type);
        break;
    }
}

static void dz_connect_on_deactivate(void*            delegate,
                                     void*            operation_userdata,
                                     dz_error_t       status,
                                     dz_object_handle result) {
    activation_count--;
    log("CONNECT deactivated - c = %d with status = %d\n",
        activation_count,
        status);
}

static void dz_player_on_deactivate(void* delegate,
                                    void* operation_userdata,
                                    dz_error_t status,
                                    dz_object_handle result) {
    activation_count--;
    log("PLAYER deactivated - c = %d with status = %d\n",
        activation_count,
        status);
}

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* deezer-track
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

#ifndef libdeezer_deezer_track_h
#define libdeezer_deezer_track_h

#include "deezer-object.h"

/**
 * @defgroup DeezerCApiMediaTrack Deezer MediaTrack C API
 * @ingroup DeezerCApi
 * @brief Access the Deezer contents informations.
 *
 * @{
 */


/**
 * @brief Deezer track qualities.
 */
typedef enum {

    DZ_TRACK_QUALITY_UNKNOWN,       /**< Track quality has not been set yet, not a valid value. */
    DZ_TRACK_QUALITY_STANDARD,      /**< Medium quality compressed audio.     */
    DZ_TRACK_QUALITY_HIGHQUALITY,   /**< High quality compressed audio.       */
    DZ_TRACK_QUALITY_CDQUALITY,     /**< Lossless two channel 44,1KHz 16bits. */
    DZ_TRACK_QUALITY_DATA_EFFICIENT,/**< Try using smaller file formats.      */

    DZ_TRACK_QUALITY_LAST_ENTRY     /**< NOT a valid value, just for internal purpose of array sizing, keep it at the last position */
} dz_track_quality_t;

/**
 * @brief Track id type definition.
 */
typedef int64_t dz_track_id_t;

/**
 * @brief Not a valid track id value. Used to init variable.
 */
#define DZ_TRACK_ID_INVALID ((dz_track_id_t)-1)


/**
 * @brief Track metadata types.
 */
typedef enum {
    DZ_TRACK_METADATA_UNKNOWN,       /**< Track metadata has not been set yet, not a valid value. */
    DZ_TRACK_METADATA_FORMAT_HEADER, /**< Track header metadata type. */
    DZ_TRACK_METADATA_DURATION_MS,   /**< Track duration metadata type. */
} dz_track_metadata_t;


/**
 * @brief An audio sample is raw pcm data over one period.
 * @note For either mono or stereo signal, we have 44,1K samples per sec on a 44,1KHz signal.
 */
typedef uint64_t dz_audiosample_t;

/**
 * @brief Number of samples per second.
 * @note Standard values are 44100, 48000, 96000, ...
 */
typedef int dz_audiosample_rate_t;

/**
 * @brief In compressed audio stream, a frame is a sequence of compressed data that can be parsed and decoded.
 */
typedef uint32_t dz_audioframe_t;

/**
 * @brief In compressed audio stream, a block is at output of decoder, made of several samples.
 */
typedef uint32_t dz_audioblock_t;

/**
 * @brief Media format of the audio track.
 */
typedef enum {
    DZ_MEDIA_FORMAT_UNKNOWN,     /**< Media format has not been set yet, not a valid value. */
    DZ_MEDIA_FORMAT_AUDIO_PCM,   /**< Audio format is PCM. */
    DZ_MEDIA_FORMAT_AUDIO_MPEG,  /**< Audio format is MPEG. */
    DZ_MEDIA_FORMAT_AUDIO_FLAC,  /**< Audio format is FLAC. */
} dz_media_format_t;

/**
 * @brief Audio sample format.
 *
 * This is a bit mask information that can carry #dz_sample_fmt_non_interleaved information.
 */
typedef uint32_t dz_audiosample_format_t;

#define dz_sample_fmt_float32         ((dz_audiosample_format_t) 0x00000001) /**< Float 32 bits packed format @see dz_audiosample_format_t. */
#define dz_sample_fmt_int32           ((dz_audiosample_format_t) 0x00000002) /**< Int   32 bits packed format @see dz_audiosample_format_t. */
#define dz_sample_fmt_int24           ((dz_audiosample_format_t) 0x00000004) /**< Int   24 bits packed format @see dz_audiosample_format_t. */
#define dz_sample_fmt_int16           ((dz_audiosample_format_t) 0x00000008) /**< Int   16 bits packed format @see dz_audiosample_format_t. */
#define dz_sample_fmt_int8            ((dz_audiosample_format_t) 0x00000010) /**< Int    8 bits packed format @see dz_audiosample_format_t. */
#define dz_sample_fmt_uint8           ((dz_audiosample_format_t) 0x00000020) /**< Unsigned Int 8 bits packed format @see dz_audiosample_format_t. */
#define dz_sample_fmt_other           ((dz_audiosample_format_t) 0x00008000) /**< Other packed format @see dz_audiosample_format_t. */

#define dz_sample_fmt_non_interleaved ((dz_audiosample_format_t) 0x80000000) /**< Non interleaved information @see dz_audiosample_format_t. */

/**
 * @brief Audio channel position.
 */
typedef uint32_t dz_channel_position_t;

#define dz_channel_position_front_left   ((dz_channel_position_t)0x00000001) /**< Front left   channel. */
#define dz_channel_position_front_right  ((dz_channel_position_t)0x00000002) /**< Front right  channel. */
#define dz_channel_position_front_center ((dz_channel_position_t)0x00000004) /**< Front center channel. */
#define dz_channel_position_lfe          ((dz_channel_position_t)0x00000008) /**< Low Frequency Effects channel. */
#define dz_channel_position_back_left    ((dz_channel_position_t)0x00000010) /**< Back left  also surround left  for 5.1 or 6.1 mapping. */
#define dz_channel_position_back_right   ((dz_channel_position_t)0x00000020) /**< Back right also surround right for 5.1 or 6.1 mapping.  */
#define dz_channel_position_side_left    ((dz_channel_position_t)0x00000040) /**< Side left  channel. */
#define dz_channel_position_side_right   ((dz_channel_position_t)0x00000080) /**< Side right channel. */

/**
 * @brief Media track detailed informations.
 */
typedef struct {
    
    dz_media_format_t format; /**< @see dz_media_format_t */
    
    union {
        struct {
            int  layer;    /**< Equals to 3 for MP3 */
            float version; /**< Equals to 1.0, 2.0 or 2.5 for layer 3 */
        } audio_mpeg;      /**< Audio MPEG information */
        
        struct {
            int dummy;     /**< Dummy field to avoid MSVC complaints */
        } audio_flac;      /**< Audio FLAC information */
    };
    bool is_cbr;           /**< Is the media encoded at a constant bitrate */
    float average_bitrate; /**< Average bitrate */
    
    union {
        struct {
            struct {
                dz_audiosample_format_t format;    /**< Sample size. */
                dz_audiosample_rate_t sample_rate; /**< Sample rate (in Hz). */
                size_t channels;                   /**< Number of channels. */
                uint32_t channel_position_mask;    /**< Channel position @see dz_channel_position_t. */
            } samples;                             /**< Samples information. */
            dz_audiosample_t total_samples;        /**< Number of samples. */
        } audio;                                   /**< Audio information. */
        struct {
            int dummy0; /**< Dummy field to avoid MSVC complaints */
        } video;
        struct {
            int dummy1; /**< Dummy field to avoid MSVC complaints */
        } subtitle;
    };
} dz_media_track_detailed_infos_t;

/**
 * @brief Track metadata handle declaration.\n
 * This handle is returned to the application through the dz_player_onmetadata_cb() callback.
 */
DECLARE_CLASS(dz_track_metadata);

/**
 * @brief Get the track metadata type.
 */
dz_track_metadata_t dz_track_metadata_get_type(dz_track_metadata_handle self);

/**
 * @brief Get the media track detailed infos from a track metadata handle.
 */
const dz_media_track_detailed_infos_t*  dz_track_metadata_get_format_header(dz_track_metadata_handle self);

/**
 * @brief Get the media track duration in milliseconds from a track metadata handle.
 */
dz_mseconds_t dz_track_metadata_get_duration(dz_track_metadata_handle self);

/** @} */ //End of group tag (for documentation purpose)

#endif

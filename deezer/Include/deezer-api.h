/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* deezer-api.self get access to Deezer REST API
 *
 * Copyright (C) 2013 Deezer
 *
 * Licensed under Deezer copyright
 *
 * @author Frederic Germain
 * @author Cyril Picheney
 */

#ifndef DEEZER_API_H
#define DEEZER_API_H

#include "deezer-connect.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DeezerCApiRestApi Deezer REST C API
 * @ingroup  DeezerCApi
 * @brief    API to make C REST call.
 * @{
 */

/**
 * @brief Deezer REST command type.
 */
typedef enum {
    DZ_API_CMD_UNKNOWN,/**< API Cmd has not been set yet, not a valid value. */
    DZ_API_CMD_GET,    /**< REST GET method. */
    DZ_API_CMD_POST,   /**< REST POST method. */
    DZ_API_CMD_DELETE  /**< REST DELETE method. */
} dz_api_cmd_t;

/**
 * @brief Deezer REST processing status.
 */
typedef enum {
    DZ_API_RESULT_UNKNOWN,        /**< API result has not been set yet, not a valid value. */
    DZ_API_RESULT_COMPLETED,      /**< The request was completed, result in responsedata. */
    DZ_API_RESULT_CANCELED,       /**< dz_api_request_processing_cancel() was called. */
    DZ_API_RESULT_IOEXCEPTION,    /**< An IO (socket, ie ip or tcp) exception occured. */
    DZ_API_RESULT_SSLEXCEPTION,   /**< A SSL (certificate, ...) exception occured. */
    DZ_API_RESULT_HTTPEXCEPTION,  /**< A HTTP (parsing,...) exception occured. */
    DZ_API_RESULT_JSONEXCEPTION,  /**< A JSON (parsing, ...) exception occured. */
    DZ_API_RESULT_TIMEOUT,        /**< The request could not complete before timeout. */
} dz_api_result_t;

/**
 * @brief Deezer REST request handle.
 *
 * Contains information about the Deezer REST request you wish to make.
 */
DECLARE_CLASS(dz_api_request);

/**
 * @brief Deezer stream tokener.
 *
 * Contains information about the stream tokener instance used by the stream parser.
 */
typedef void* dz_stream_tokener;

/**
 * @brief Deezer stream object.
 *
 * Contains information about the final stream object returned by the #dz_api_request_done_cb.\n
 * This object instance is the one that has been parsed by the stream parser #dz_stream_parser_class instance.
 */
typedef void* dz_stream_object;

/**
 * @brief Deezer stream parser interface handle.
 *
 * An optional parser can be passed to the Native SDK to retrieve big JSON responses
 * by chunks instead of having one huge string.\n
 * The role of the class implementing this interface is to convert the JSON string
 * into the expected structure.
 *
 * For instance, you could use it to convert JSON strings to dictionaries.
 */
typedef struct dz_stream_parser_class {
    dz_stream_tokener (*tokener_new)(void);
    void              (*tokener_free)(dz_stream_tokener tok);
    dz_stream_object  (*tokener_parse)(dz_stream_tokener tok, const char*data, size_t len);
    void              (*tokener_object_release)(dz_stream_object obj);
} *dz_stream_parser_class;

/**
 * @brief Deezer REST request processing handle.
 *
 * Represent a REST request being processed.
 */
DECLARE_CLASS(dz_api_request_processing);

/**
 * @brief Constructor for dz_api_request_handle.
 *
 * @param   cmdtype     GET, POST or DELETE.
 * @param   rest_object A path to a REST object ("/album/302127").
 * @returns             A request handle.
 */
LIBDEEZER_API dz_api_request_handle dz_api_request_new(dz_api_cmd_t cmdtype,
                                                       const char*  rest_object);

/**
 * @brief Add a query string (GET) or a field in POST data.
 *
 * @note Call should be done just after #dz_api_request_new.
 *
 * @param self  Deezer request handle.
 * @param key   Field key to add, should be a const static char*.
 * @param value Field value to add.
 * @returns     A request handle.
 */
LIBDEEZER_API dz_error_t dz_api_request_add_string_parameter(dz_api_request_handle self,
                                                             const char* key,
                                                             const char* value);


/**
 * @brief Prototype of callback called by an async request #dz_api_request_processing_handle.
 * @param responsedata  A stream object which is the JSON object processed by the #dz_stream_parser_class
 *                      provided to the dz_api_request_processing_async() or the JSON as a raw C-string
 *                      if the no stream parser has been provided.
 */
typedef void (*dz_api_request_done_cb)(dz_api_request_processing_handle handle,
                                       dz_api_result_t  ret,
                                       dz_stream_object responsedata,
                                       void*            userdata
                                      );

/**
 * @brief Launch an async request process.
 *
 * The callback is called once with an error status or with the JSON object.
 * The #dz_api_request_processing_handle should be released after
 * the callback is called.
 *
 * @param connect  A #dz_connect_handle (if NULL, use anonymous session).
 * @param request  A #dz_api_request_handle representing a request to be issued.\n
 *                 This request will be made asynchronously, and as such will
 *                 be stored in a queue.\n
 *                 Therefore the SDK will retain a reference to it,
 *                 so make sure to release it when you don't need it anymore.
 * @param cb       The callback, it will always be called once.
 * @param userdata A reference to the user’s data.
 * @param parser   (Optional) A stream parser interface #dz_stream_parser_class implementation handle. This is optional, it can be let to NULL. If it is let NULL, the responsedata from dz_api_request_done_cb() will be returned as a raw JSON.
 * @returns        A request processing handle.
 */
LIBDEEZER_API dz_api_request_processing_handle dz_api_request_processing_async(dz_connect_handle connect,
                                                                                dz_api_request_handle  request,
                                                                                dz_stream_parser_class parser,
                                                                                dz_api_request_done_cb cb,
                                                                                void*                  userdata);

/**
 * @brief Cancel an async request processing.
 *
 * The processing callback is called as soon as possible.
 *
 * @param reqproc The request processing to cancel.
 */
LIBDEEZER_API void dz_api_request_processing_cancelme(dz_api_request_processing_handle reqproc);

/** @} */ //End of group tag (for documentation purpose)

#ifdef __cplusplus
}
#endif

#endif

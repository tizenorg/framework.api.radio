/*
* Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __TIZEN_MEDIA_RADIO_H__
#define __TIZEN_MEDIA_RADIO_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file radio.h
 * @brief This file contains the radio API.
 * @since_tizen 2.3
 */

/**
 * @addtogroup CAPI_MEDIA_RADIO_MODULE
 * @{
 */

/**
 * @brief Radio type handle.
 * @since_tizen 2.3
 */
typedef struct radio_s *radio_h;

/**
 * @brief Enumeration of radio state.
 * @since_tizen 2.3
 */
typedef enum
{
	RADIO_STATE_READY,			/**< Ready to play or scan */
	RADIO_STATE_PLAYING,		/**< Playing the audio from the tuner */
	RADIO_STATE_SCANNING,		/**< Scanning/Searching for the next station signal that starts from a given starting frequency */
} radio_state_e;

/**
 * @brief Enumeration of error codes for the radio.
 * @since_tizen 2.3
 */
typedef enum
{
	RADIO_ERROR_NONE		        = TIZEN_ERROR_NONE,								/**< Successful */
	RADIO_ERROR_OUT_OF_MEMORY	    = TIZEN_ERROR_OUT_OF_MEMORY,				/**< Out of memory */
	RADIO_ERROR_INVALID_PARAMETER  = TIZEN_ERROR_INVALID_PARAMETER,			/**< Invalid parameter */
	RADIO_ERROR_INVALID_OPERATION	= TIZEN_ERROR_INVALID_OPERATION,			/**< Invalid operation */
	RADIO_ERROR_INVALID_STATE	    = TIZEN_ERROR_RADIO | 0x01	,					/**< Invalid state */
	RADIO_ERROR_SOUND_POLICY	    = TIZEN_ERROR_RADIO | 0x02	,					/**< Sound policy error */
	RADIO_ERROR_PERMISSION_DENIED   = TIZEN_ERROR_PERMISSION_DENIED,			/**< Permission denied */
	RADIO_ERROR_NOT_SUPPORTED   = TIZEN_ERROR_NOT_SUPPORTED,					/**< Not supported */
} radio_error_e;

/**
 * @brief Enumeration of radio interrupted type.
 * @since_tizen 2.3
 */
typedef enum
{
       RADIO_INTERRUPTED_COMPLETED = 0,					/**< Interrupt completed */
       RADIO_INTERRUPTED_BY_MEDIA,						/**< Interrupted by a non-resumable media application */
       RADIO_INTERRUPTED_BY_CALL,						/**< Interrupted by an incoming call */
       RADIO_INTERRUPTED_BY_EARJACK_UNPLUG,			/**< Interrupted by unplugging headphones */
       RADIO_INTERRUPTED_BY_RESOURCE_CONFLICT,		/**< Interrupted by a resource conflict */
       RADIO_INTERRUPTED_BY_ALARM,						/**< Interrupted by an alarm */
       RADIO_INTERRUPTED_BY_EMERGENCY,				/**< Interrupted by an emergency */
       RADIO_INTERRUPTED_BY_RESUMABLE_MEDIA,			/**< Interrupted by a resumable media application */
       RADIO_INTERRUPTED_BY_NOTIFICATION,				/**< Interrupted by a notification */
} radio_interrupted_code_e;

/**
 * @brief  Called when the scan information is updated.
 * @since_tizen 2.3
 * @param[in] frequency The tuned radio frequency [87500 ~ 108000] (kHz)
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked by radio_scan_start().
 * @see radio_scan_start()
 */
typedef void (*radio_scan_updated_cb)(int frequency, void *user_data);

/**
 * @brief  Called when the radio scan is stopped.
 * @since_tizen 2.3
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked when the scan is stopped by radio_scan_stop().
 * @see radio_scan_stop()
 */
typedef void (*radio_scan_stopped_cb)(void *user_data);

/**
 * @brief  Called when the radio scan is completed.
 * @since_tizen 2.3
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked when the scan is completed by registering this callback using radio_set_scan_completed_cb().
 * @see radio_scan_start()
 * @see radio_set_scan_completed_cb()
 * @see radio_unset_scan_completed_cb()
 */
typedef void (*radio_scan_completed_cb)(void *user_data);

/**
 * @brief  Called when the radio seek is completed.
 * @since_tizen 2.3
 * @param[in] frequency The current frequency [87500 ~ 108000] (kHz)
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked when the radio seek is completed by registering this callback using radio_seek_up() or radio_seek_down().
 * @see radio_seek_up()
 * @see radio_seek_down()
 */
typedef void (*radio_seek_completed_cb)(int frequency, void *user_data);

/**
 * @brief  Called when the radio is interrupted.
 * @since_tizen 2.3
 * @param[in]	error_code	The interrupted error code
 * @param[in]	user_data	The user data passed from the callback registration function
 * @see radio_set_interrupted_cb()
 * @see radio_unset_interrupted_cb()
 */
typedef void (*radio_interrupted_cb)(radio_interrupted_code_e code, void *user_data);

/**
 * @brief Creates a radio handle.
 * @since_tizen 2.3
 * @remarks You must release @a radio using radio_destroy().
 * @param[out]  radio  A new handle to radio
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_destroy()
 */
int radio_create(radio_h *radio);

/**
 * @brief Destroys the radio handle and releases all its resources.
 * @since_tizen 2.3
 * @remarks To completely shutdown the radio operation, call this function with a valid radio handle.
 *
 * @param[in]		radio The handle to radio to be destroyed
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_create()
 */
int radio_destroy(radio_h radio);

/**
 * @brief Gets the radio's current state.
 * @since_tizen 2.3
 * @param[in]   radio	The handle to radio
 * @param[out]  state	The current state of the radio
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 */
int  radio_get_state(radio_h radio, radio_state_e *state);

/**
 * @brief Starts playing the radio.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @retval #RADIO_ERROR_SOUND_POLICY Sound policy error
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @pre The radio state must be set to #RADIO_STATE_READY by calling radio_create().
 * @post The radio state will be #RADIO_STATE_PLAYING.
 * @see radio_stop()
 */
int radio_start(radio_h radio);

/**
 * @brief Stops playing the radio.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid state
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @pre The radio state must be set to #RADIO_STATE_PLAYING by calling radio_start().
 * @post The radio state will be #RADIO_STATE_READY.
 * @see radio_start()
 * @see radio_scan_start()
 */
int radio_stop(radio_h radio);

/**
 * @brief Seeks up the effective frequency of the radio, asynchronously.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @pre The radio state must be set to #RADIO_STATE_PLAYING by calling radio_start().
 * @post It invokes radio_seek_completed_cb() when the seek completes.
 * @see radio_seek_down()
 */
int radio_seek_up(radio_h radio,radio_seek_completed_cb callback, void *user_data );

/**
 * @brief Seeks down the effective frequency of the radio, asynchronously.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @pre The radio state must be set to #RADIO_STATE_PLAYING by calling radio_start().
 * @post It invokes radio_seek_completed_cb() when the seek completes.
 * @see radio_seek_up()
 */
int radio_seek_down(radio_h radio,radio_seek_completed_cb callback, void *user_data );

/**
 * @brief Sets the radio frequency.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @param[in]   frequency The frequency to set [87500 ~ 108000] (kHz)
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_get_frequency()
 */
int radio_set_frequency(radio_h radio, int frequency);

/**
 * @brief Gets the current frequency of the radio.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @param[out]  frequency The current frequency [87500 ~ 108000] (kHz)
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_set_frequency()
 */
int radio_get_frequency(radio_h radio, int *frequency);

/**
 * @brief Gets the current signal strength of the radio.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @param[out]  strength The current signal strength [-128 ~ 128] (dBm)
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 */
int radio_get_signal_strength(radio_h radio, int *strength);

/**
 * @brief Starts scanning radio signals, asynchronously
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @pre The radio state must be set to #RADIO_STATE_READY by calling radio_create() or radio_stop().
 * @post The radio state will be #RADIO_STATE_SCANNING during a search. After the scan is completed, the radio state will be #RADIO_STATE_READY.
 * @post It invokes radio_scan_updated_cb() when the scan information updates.
 * @post It invokes radio_scan_completed_cb() when the scan completes, if you set a callback with radio_set_scan_completed_cb().
 * @see radio_scan_stop()
 * @see radio_set_scan_completed_cb()
 * @see radio_scan_completed_cb()
 */
int radio_scan_start(radio_h radio, radio_scan_updated_cb callback, void *user_data);

/**
 * @brief Stops scanning radio signals, asynchronously.
 * @since_tizen 2.3
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid state
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @pre The radio state must be set to #RADIO_STATE_SCANNING by calling radio_scan_start().
 * @post It invokes radio_scan_stopped_cb() when the scan stops.
 * @post The radio state will be #RADIO_STATE_READY.
 * @see radio_scan_start()
 */
int radio_scan_stop(radio_h radio, radio_scan_stopped_cb callback, void *user_data);

/**
 * @brief Sets the radio's mute status.
 * @since_tizen 2.3
 * @details  If the mute status is @c true, no sounds will be played. If @c false, sounds will be played. Until this function is called, by default the radio is not muted.
 * @param[in]   radio The handle to radio
 * @param[in]   muted The new mute status: (@c true = mute, @c false = not muted)
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_is_muted()
 */
int radio_set_mute(radio_h radio, bool muted);

/**
 * @brief Gets the radio's mute status.
 * @since_tizen 2.3
 * @details If the mute status is @c true, no sounds are played. If @c false, sounds are played.
 * @param[in]   radio The handle to radio
 * @param[out]  muted  The current mute status: (@c true = mute, @c false = not muted)
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_set_mute()
 */
int radio_is_muted(radio_h radio, bool *muted);

/**
 * @brief Registers a callback function to be invoked when the scan finishes.
 * @since_tizen 2.3
 * @param[in] radio	The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @post radio_scan_completed_cb() will be invoked.
 * @see radio_unset_scan_completed_cb()
 * @see radio_scan_completed_cb()
 */
int radio_set_scan_completed_cb(radio_h radio, radio_scan_completed_cb callback, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen 2.3
 * @param[in] radio The handle to radio
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_set_scan_completed_cb()
 */
int radio_unset_scan_completed_cb(radio_h radio);

/**
 * @brief Registers a callback function to be invoked when the radio is interrupted.
 * @since_tizen 2.3
 * @param[in] radio	The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @post  radio_interrupted_cb() will be invoked.
 * @see radio_unset_interrupted_cb()
 * @see #radio_interrupted_code_e
 * @see radio_interrupted_cb()
 */
int radio_set_interrupted_cb(radio_h radio, radio_interrupted_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen 2.3
 * @param[in] radio The handle to radio
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_NOT_SUPPORTED Not supported
 * @see radio_set_interrupted_cb()
 */
int radio_unset_interrupted_cb(radio_h radio);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MEDIA_RADIO_H__ */

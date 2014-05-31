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

#define RADIO_ERROR_CLASS          TIZEN_ERROR_MULTIMEDIA_CLASS | 0x70

/**
 * @file radio.h
 * @brief This file contains the radio API.
 */

/**
 * @addtogroup CAPI_MEDIA_RADIO_MODULE
 * @{
 */

/**
 * @brief Radio handle type.
 */
typedef struct radio_s *radio_h;

/**
 * @brief Enumerations of radio state
 */
typedef enum
{
	RADIO_STATE_READY,			/**< Ready to play or scan */
	RADIO_STATE_PLAYING,		/**< Playing audio from the tuner */
	RADIO_STATE_SCANNING,		/**< Scanning Searching for the next station signal starts from a given starting frequency */
} radio_state_e;

/**
 * @brief Error codes for radio
 */
typedef enum
{
		RADIO_ERROR_NONE		        = TIZEN_ERROR_NONE,				           						 /**< Successful */
		RADIO_ERROR_OUT_OF_MEMORY	    = TIZEN_ERROR_OUT_OF_MEMORY,			    /**< Out of memory */
		RADIO_ERROR_INVALID_PARAMETER  = TIZEN_ERROR_INVALID_PARAMETER,		/**< Invalid parameter */
		RADIO_ERROR_INVALID_OPERATION	= TIZEN_ERROR_INVALID_OPERATION,		/**< Invalid operation */
		RADIO_ERROR_INVALID_STATE	    = RADIO_ERROR_CLASS | 0x01	,		    		/**< Invalid state */
		RADIO_ERROR_SOUND_POLICY	    = RADIO_ERROR_CLASS | 0x02	,		    		/**< Sound policy error */
} radio_error_e;

/**
 * @brief Enumerations of radio interrupted type
 */
typedef enum
{
       RADIO_INTERRUPTED_BY_MEDIA = 0, 				/**< Interrupted by non-resumable media application*/
       RADIO_INTERRUPTED_BY_CALL_START,						/**< Interrupted by call starting*/
       RADIO_INTERRUPTED_BY_EARJACK_UNPLUG,			/**< Interrupted by unplugging headphone*/
       RADIO_INTERRUPTED_BY_RESOURCE_CONFLICT,		/**< Interrupted by resource conflict*/
       RADIO_INTERRUPTED_BY_ALARM_START,					/**< Interrupted by alarm starting*/
       RADIO_INTERRUPTED_BY_ALARM_END,						/**< Interrupted by alarm ending*/
       RADIO_INTERRUPTED_BY_EMERGENCY,						/**< Interrupted by emergency*/
       RADIO_INTERRUPTED_BY_RESUMABLE_MEDIA,				/**< Interrupted by resumable media application*/
} radio_interrupted_code_e;

/**
 * @brief  Called when the scan information is updated.
 * @param[in] frequency The tuned radio frequency [87500 ~ 108000] (kHz)
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked by radio_scan_start()
 * @see radio_scan_start()
 */
typedef void (*radio_scan_updated_cb)(int frequency, void *user_data);

/**
 * @brief  Called when the radio scan is stopped.
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked when scan is stopped by radio_scan_stop()
 * @see radio_scan_stop()
 */
typedef void (*radio_scan_stopped_cb)(void *user_data);

/**
 * @brief  Called when the radio scan is completed.
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked when scan is completed if you register this callback using radio_set_scan_completed_cb()
 * @see radio_scan_start()
 * @see radio_set_scan_completed_cb()
 * @see radio_unset_scan_completed_cb()
 */
typedef void (*radio_scan_completed_cb)(void *user_data);

/**
 * @brief  Called when the radio seek is completed.
 * @param[in] frequency The current frequency [87500 ~ 108000] (kHz)
 * @param[in] user_data  The user data passed from the callback registration function
 * @pre It will be invoked when radio seek completed if you register this callback using radio_seek_up() or radio_seek_down()
 * @see radio_seek_up()
 * @see radio_seek_down()
 */
typedef void (*radio_seek_completed_cb)(int frequency, void *user_data);

/**
 * @brief  Called when the radio is interrupted.
 * @param[in]	error_code	The interrupted error code
 * @param[in]	user_data	The user data passed from the callback registration function
 * @see radio_set_interrupted_cb()
 * @see radio_unset_interrupted_cb()
 */
typedef void (*radio_interrupted_cb)(radio_interrupted_code_e code, void *user_data);

/**
 * @brief Creates a radio handle.
 * @remarks @a radio must be released radio_destroy() by you.
 * @param[out]  radio  A new handle to radio
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @see radio_destroy()
 */
int radio_create(radio_h *radio);

/**
 * @brief Destroys the radio handle and releases all its resources.
 *
 * @remarks To completely shutdown radio operation, call this function with a valid radio handle.
 *
 * @param[in]		radio The handle to radio to be destroyed
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @see radio_create()
 */
int radio_destroy(radio_h radio);

/**
 * @brief Gets the radio's current state.
 * @param[in]   radio	The handle to radio
 * @param[out]  state	The current state of the radio
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 */
int  radio_get_state(radio_h radio, radio_state_e *state);

/**
 * @brief Starts playing radio.
 *
 * @param[in]   radio The handle to radio
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @retval #RADIO_ERROR_SOUND_POLICY Sound policy error
 * @pre The radio state must be #RADIO_STATE_READY by radio_create().
 * @post The radio state will be #RADIO_STATE_PLAYING.
 * @see radio_stop()
 */
int radio_start(radio_h radio);

/**
 * @brief Stops playing radio.
 * @param[in]   radio The handle to radio
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid state
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @pre The radio state must be either #RADIO_STATE_PLAYING by radio_start().
 * @post The radio state will be #RADIO_STATE_READY.
 * @see radio_start()
 * @see radio_scan_start()
 */
int radio_stop(radio_h radio);

/**
 * @brief Seeks up the effective frequency of radio, asynchronously.
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @pre The radio state must be #RADIO_STATE_PLAYING by radio_start().
 * @post It invokes radio_seek_completed_cb() when seek completes.
 * @see radio_seek_down()
 */
int radio_seek_up(radio_h radio,radio_seek_completed_cb callback, void *user_data );

/**
 * @brief Seeks down the effective frequency of radio, asynchronously.
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @pre The radio state must be #RADIO_STATE_PLAYING by radio_start().
 * @post It invokes radio_seek_completed_cb() when seek completes.
 * @see radio_seek_up()
 */
int radio_seek_down(radio_h radio,radio_seek_completed_cb callback, void *user_data );

/**
 * @brief Sets the radio frequency.
 * @param[in]   radio The handle to radio
 * @param[in]   percent The frequency to set [87500 ~ 108000] (kHz)
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @see radio_get_frequency()
 */
int radio_set_frequency(radio_h radio, int frequency);

/**
 * @brief Gets the current frequency of radio. 
 * @param[in]   radio The handle to radio
 * @param[out]  frequency The current frequency [87500 ~ 108000] (kHz)
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @see radio_set_frequency()
 */
int radio_get_frequency(radio_h radio, int *frequency);

/**
 * @brief Gets the current signal strength of radio.
 * @param[in]   radio The handle to radio
 * @param[out]  strength The current signal strength [0 ~ 65535] (dbuV)
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 */
int radio_get_signal_strength(radio_h radio, int *strength);

/**
 * @brief Starts scanning radio signals, asynchronously
 *
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @pre The radio state must be #RADIO_STATE_READY by either radio_create() or radio_stop().
 * @post The radio state will be #RADIO_STATE_SCANNING during searching. After scan is completed, radio state will be #RADIO_STATE_READY.
 * @post It invokes radio_scan_updated_cb() when the scan information updates.
 * @post It invokes radio_scan_completed_cb() when scan completes, if you set a callback with radio_set_scan_completed_cb().
 * @see radio_scan_stop()
 * @see radio_set_scan_completed_cb()
 * @see radio_scan_completed_cb()
 */
int radio_scan_start(radio_h radio, radio_scan_updated_cb callback, void *user_data);

/**
 * @brief Stops scanning radio signals, asynchronously.
 * @param[in]   radio The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid state
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @retval #RADIO_ERROR_INVALID_STATE Invalid radio state
 * @pre The radio state must be #RADIO_STATE_SCANNING by radio_scan_start().
 * @post It invokes radio_scan_stopped_cb() when the scan stops.
 * @post The radio state will be #RADIO_STATE_READY.
 * @see radio_scan_start()
 */
int radio_scan_stop(radio_h radio, radio_scan_stopped_cb callback, void *user_data);

/**
 * @brief Sets the radio's mute status.
 * @details  If the mute status is @c true, no sounds will be played. If @c false, sounds will be played. Until this function is called, by default the radio is not muted.
 * @param[in]   radio The handle to radio
 * @param[in]   muted New mute status: (@c true = mute, @c false = not muted)
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @see radio_is_muted()
 */
int radio_set_mute(radio_h radio, bool muted);

/**
 * @brief Gets the radio's mute status.
 * @details If the mute status is @c true, no sounds are played. If @c false, sounds are played.
 * @param[in]   radio The handle to radio
 * @param[out]  muted  The current mute status: (@c true = mute, @c false = not muted)
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @see radio_set_mute()
 */
int radio_is_muted(radio_h radio, bool *muted);

/**
 * @brief Registers a callback function to be invoked when the scan finishes.
 * @param[in] radio	The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @post  radio_scan_completed_cb() will be invoked
 * @see radio_unset_scan_completed_cb()
 * @see radio_scan_completed_cb()
 */
int radio_set_scan_completed_cb(radio_h radio, radio_scan_completed_cb callback, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @param[in] radio The handle to radio
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @see radio_set_scan_completed_cb()
 */
int radio_unset_scan_completed_cb(radio_h radio);

/**
 * @brief Registers a callback function to be invoked when the radio is interrupted.
 * @param[in] radio	The handle to radio
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
 * @post  radio_interrupted_cb() will be invoked
 * @see radio_unset_interrupted_cb()
 * @see #radio_interrupted_code_e
 * @see radio_interrupted_cb()
 */
int radio_set_interrupted_cb(radio_h radio, radio_interrupted_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @param[in] radio The handle to radio
 * @return 0 on success, otherwise a negative error value.
 * @retval #RADIO_ERROR_NONE Successful
 * @retval #RADIO_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #RADIO_ERROR_INVALID_OPERATION Invalid operation
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

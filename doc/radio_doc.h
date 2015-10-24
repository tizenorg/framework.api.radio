/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_MEDIA_RADIO_DOC_H__
#define __TIZEN_MEDIA_RADIO_DOC_H__


/**
 * @file radio_doc.h
 * @brief This file contains high level documentation of the Multimedia Service.
 */

/**
 * @ingroup CAPI_MEDIA_FRAMEWORK
 * @defgroup CAPI_MEDIA_RADIO_MODULE Radio
 * @brief  The @ref CAPI_MEDIA_RADIO_MODULE APIs provide functions for accessing the radio.
 *
 * @section CAPI_MEDIA_RADIO_MODULE_HEADER Required Header
 *   \#include <radio.h>
 *
 * @section CAPI_MEDIA_RADIO_MODULE_OVERVIEW Overview
 *
 * The Radio API provides support for using the Radio.
 * The API allows :
 * - Starting and stopping the radio
 * - Seeking radio frequency
 * - Scanning radio signals
 * - Getting the state of the radio
 *
 * A radio handle (#radio_h) is created by calling the radio_create() function and can be started by using the radio_start() function.
 * It provides functions to start (radio_scan_start()) and stop (radio_scan_stop()) radio signal scanning. The radio frequency
 * seek up and seek down can be done asynchronously by calling the radio_seek_up() and radio_seek_down() functions respectively.
 * It also provides functions to get (radio_get_frequency()) and set (radio_set_frequency()) frequency for the given radio handle.
 *
 * @subsection CAPI_MEDIA_RADIO_LIFE_CYCLE_STATE_DIAGRAM State Diagram
 * The radio API is controlled by a state machine.
 * The following diagram shows the life cycle and the states of the Radio.
 * @image html capi_media_radio_state_diagram.png
 *
 *
 * @subsection CAPI_MEDIA_RADIO_LIFE_CYCLE_STATE_TRANSITIONS State Transitions
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>FUNCTION</b></th>
 *        <th><b>PRE-STATE</b></th>
 *        <th><b>POST-STATE</b></th>
 *        <th><b>SYNC TYPE</b></th>
 *     </tr>
 *     <tr>
 *        <td>radio_create()</td>
 *        <td>NONE</td>
 *        <td>READY</td>
 *        <td>SYNC</td>
 *     </tr>
 *     <tr>
 *         <td>radio_destroy()</td>
 *         <td>READY</td>
 *         <td>NONE</td>
 *         <td>SYNC</td>
 *     </tr>
 *     <tr>
 *        <td>radio_start()</td>
 *        <td>READY</td>
 *        <td>PLAYING</td>
 *        <td>ASYNC</td>
 *     </tr>
 *     <tr>
 *        <td>radio_stop()</td>
 *        <td>PLAYING</td>
 *        <td>READY</td>
 *        <td>ASYNC</td>
 *     </tr>
 *     <tr>
 *        <td>radio_scan_start()</td>
 *        <td>READY</td>
 *        <td>SCANNING</td>
 *        <td>ASYNC</td>
 *     </tr>
 *     <tr>
 *        <td>radio_scan_stop()</td>
 *        <td>SCANNING</td>
 *        <td>READY</td>
 *        <td>ASYNC</td>
 *     </tr>
 *     <tr>
 *        <td>radio_seek_up()</td>
 *        <td>PLAYING</td>
 *        <td>PLAYING</td>
 *        <td>SYNC</td>
 *     </tr>
 *     <tr>
 *        <td>radio_seek_down()</td>
 *        <td>PLAYING</td>
 *        <td>PLAYING</td>
 *        <td>SYNC</td>
 *     </tr>
 * </table></div>
 *
 *
 * This API also gives notifications for radio's state change events by a callback mechanism.
 * @subsection CAPI_MEDIA_RADIO_LIFE_CYCLE_CALLBACK_OPERATIONS Callback(Event) Operations
 * The callback mechanism is used to notify the application about radio events.
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>REGISTER</b></th>
 *        <th><b>UNREGISTER</b></th>
 *        <th><b>CALLBACK</b></th>
 *        <th><b>DESCRIPTION</b></th>
 *     </tr>
 *     <tr>
 *        <td>radio_set_scan_completed_cb()</td>
 *        <td>radio_unset_scan_completed_cb()</td>
 *        <td> radio_scan_completed_cb()</td>
 *        <td>This callback is invoked when the scan is completed </td>
 *     </tr>
 *		 <tr>
 *        <td>radio_set_interrupted_cb()</td>
 *        <td>radio_unset_interrupted_cb()</td>
 *        <td>radio_interrupted_cb()</td>
 *        <td>This callback is used to notify when the radio is interrupted </td>
 *     </tr>
 *</table></div>
  * @section CAPI_MEDIA_RADIO_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/fmradio\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 *
 *
 */

#endif /* __TIZEN_MEDIA_RADIO_DOC_H__ */

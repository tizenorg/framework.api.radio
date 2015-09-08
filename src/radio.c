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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mm_types.h>
#include <radio_private.h>
#include <dlog.h>
#include <glib.h>
#include <system_info.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_RADIO"

/*
* Internal Macros
*/
#define RADIO_CHECK_CONDITION(condition,error,msg)	\
		if(condition) {} else \
		{ LOGE("[%s] %s(0x%08x)",(char*)__FUNCTION__, msg,error); return error;}; \

#define RADIO_INSTANCE_CHECK(radio)	\
	RADIO_CHECK_CONDITION(radio != NULL, RADIO_ERROR_INVALID_PARAMETER,"RADIO_ERROR_INVALID_PARAMETER")

#define RADIO_STATE_CHECK(radio,expected_state)	\
	RADIO_CHECK_CONDITION(radio->state == expected_state,RADIO_ERROR_INVALID_STATE,"RADIO_ERROR_INVALID_STATE")

#define RADIO_NULL_ARG_CHECK(arg)	\
	RADIO_CHECK_CONDITION(arg != NULL,RADIO_ERROR_INVALID_PARAMETER,"RADIO_ERROR_INVALID_PARAMETER")

#define RADIO_SUPPORT_CHECK(arg)	\
	RADIO_CHECK_CONDITION(arg != false, RADIO_ERROR_NOT_SUPPORTED,"RADIO_ERROR_NOT_SUPPORTED")

/*
* Internal Implementation
*/
static int __convert_error_code(int code, char *func_name)
{
	int ret = RADIO_ERROR_NONE;
	char* msg="RADIO_ERROR_NONE";
	LOGI("[%s] Enter code :%x", __func__, code);
	switch(code)
	{
		case MM_ERROR_NONE:
			ret = RADIO_ERROR_NONE;
			msg = "RADIO_ERROR_NONE";
			break;

		case  MM_ERROR_RADIO_NO_FREE_SPACE:
			ret = RADIO_ERROR_OUT_OF_MEMORY;
			msg = "RADIO_ERROR_OUT_OF_MEMORY";
			break;
		case MM_ERROR_RADIO_NOT_INITIALIZED:
		case MM_ERROR_RADIO_NO_OP:
		case MM_ERROR_RADIO_INVALID_STATE:
			ret = RADIO_ERROR_INVALID_STATE;
			msg = "RADIO_ERROR_INVALID_STATE";
			break;
		case MM_ERROR_COMMON_INVALID_ARGUMENT:
			ret = RADIO_ERROR_INVALID_PARAMETER;
			msg = "RADIO_ERROR_INVALID_PARAMETER";
			break;
		case MM_ERROR_POLICY_BLOCKED:
		case MM_ERROR_POLICY_INTERRUPTED:
		case MM_ERROR_POLICY_INTERNAL:
		case MM_ERROR_POLICY_DUPLICATED:
			ret = RADIO_ERROR_SOUND_POLICY;
			msg = "RADIO_ERROR_SOUND_POLICY";
			break;
		case  MM_ERROR_RADIO_INTERNAL:
		case  MM_ERROR_RADIO_RESPONSE_TIMEOUT:
			ret= RADIO_ERROR_INVALID_OPERATION;
			msg = "RADIO_ERROR_INVALID_OPERATION";
			break;
		case  MM_ERROR_RADIO_DEVICE_NOT_FOUND:
			ret = RADIO_ERROR_NOT_SUPPORTED;
			msg = "RADIO_ERROR_NOT_SUPPORTED";
			break;
		case  MM_ERROR_RADIO_DEVICE_NOT_OPENED:
		default :
			ret= RADIO_ERROR_PERMISSION_DENIED;
			msg = "RADIO_ERROR_PERMISSION_DENIED";
	}
	LOGE("[%s] %s(0x%08x) : core fw error(0x%x)",func_name,msg, ret, code);
	return ret;
}

static radio_state_e __convert_radio_state(MMRadioStateType state)
{
	int converted_state = RADIO_STATE_READY;
	LOGI("[%s] Enter state: %d", __func__, state);
	switch(state)
	{

		case MM_RADIO_STATE_PLAYING:
			converted_state = RADIO_STATE_PLAYING;
			break;
		case MM_RADIO_STATE_SCANNING:
			converted_state = RADIO_STATE_SCANNING;
			break;
		case MM_RADIO_STATE_NULL:
		case MM_RADIO_STATE_READY:
		default:
			 converted_state = RADIO_STATE_READY;
			 break;
	}
	LOGI("[%s] Leave converted_state: %d", __func__, converted_state);
	return converted_state;
}


static radio_interrupted_code_e __convert_interrupted_code(int code)
{
	LOGI("[%s] Enter code: %d", __func__, code);
	radio_interrupted_code_e ret = RADIO_INTERRUPTED_BY_RESOURCE_CONFLICT;
	switch(code)
	{
		case MM_MSG_CODE_INTERRUPTED_BY_CALL_END:
		case MM_MSG_CODE_INTERRUPTED_BY_ALARM_END:
		case MM_MSG_CODE_INTERRUPTED_BY_EMERGENCY_END:
		case MM_MSG_CODE_INTERRUPTED_BY_NOTIFICATION_END:
			ret = RADIO_INTERRUPTED_COMPLETED;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_MEDIA:
		case MM_MSG_CODE_INTERRUPTED_BY_OTHER_PLAYER_APP:
			ret = RADIO_INTERRUPTED_BY_MEDIA;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_CALL_START:
			ret = RADIO_INTERRUPTED_BY_CALL;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_EARJACK_UNPLUG:
			ret = RADIO_INTERRUPTED_BY_EARJACK_UNPLUG;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_ALARM_START:
			ret = RADIO_INTERRUPTED_BY_ALARM;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_NOTIFICATION_START:
			ret = RADIO_INTERRUPTED_BY_NOTIFICATION;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_EMERGENCY_START:
			ret = RADIO_INTERRUPTED_BY_EMERGENCY;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_RESUMABLE_MEDIA:
			ret = RADIO_INTERRUPTED_BY_RESUMABLE_MEDIA;
			break;
		case MM_MSG_CODE_INTERRUPTED_BY_RESOURCE_CONFLICT:
		default :
			ret = RADIO_INTERRUPTED_BY_RESOURCE_CONFLICT;
			break;
	}
	LOGE("[%s] interrupted code(%d) => ret(%d)",__FUNCTION__,code, ret);
	return ret;
}

static int __set_callback(_radio_event_e type, radio_h radio, void* callback, void *user_data)
{
	RADIO_INSTANCE_CHECK(radio);
	RADIO_NULL_ARG_CHECK(callback);
	radio_s * handle = (radio_s *) radio;
	handle->user_cb[type] = callback;
	handle->user_data[type] = user_data;
	LOGI("[%s] Event type : %d ",__FUNCTION__, type);
	return RADIO_ERROR_NONE;
}

static int __unset_callback(_radio_event_e type, radio_h radio)
{
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;
	handle->user_cb[type] = NULL;
	handle->user_data[type] = NULL;
	LOGI("[%s] Event type : %d ",__FUNCTION__, type);
	return RADIO_ERROR_NONE;
}

static int __msg_callback(int message, void *param, void *user_data)
{
	radio_s * handle = (radio_s*)user_data;
	MMMessageParamType *msg = (MMMessageParamType*)param;
	LOGI("[%s] Got message type : 0x%x" ,__FUNCTION__, message);
	switch(message)
	{
		case MM_MESSAGE_RADIO_SCAN_INFO:
			if( handle->user_cb[_RADIO_EVENT_TYPE_SCAN_INFO] )
			{
				((radio_scan_updated_cb)handle->user_cb[_RADIO_EVENT_TYPE_SCAN_INFO])(msg->radio_scan.frequency,handle->user_data[_RADIO_EVENT_TYPE_SCAN_INFO]);
			}
			break;
		case MM_MESSAGE_RADIO_SCAN_STOP:
			if( handle->user_cb[_RADIO_EVENT_TYPE_SCAN_STOP] )
			{
				((radio_scan_stopped_cb)handle->user_cb[_RADIO_EVENT_TYPE_SCAN_STOP])(handle->user_data[_RADIO_EVENT_TYPE_SCAN_STOP]);
			}
			break;
		case MM_MESSAGE_RADIO_SCAN_FINISH:
			if( handle->user_cb[_RADIO_EVENT_TYPE_SCAN_FINISH] )
			{
				((radio_scan_completed_cb)handle->user_cb[_RADIO_EVENT_TYPE_SCAN_FINISH])(handle->user_data[_RADIO_EVENT_TYPE_SCAN_FINISH]);
			}
			break;
		case MM_MESSAGE_RADIO_SEEK_FINISH:
			if( handle->user_cb[_RADIO_EVENT_TYPE_SEEK_FINISH] )
			{
				((radio_seek_completed_cb)handle->user_cb[_RADIO_EVENT_TYPE_SEEK_FINISH])(msg->radio_scan.frequency, handle->user_data[_RADIO_EVENT_TYPE_SEEK_FINISH]);
			}
			break;
		case MM_MESSAGE_STATE_INTERRUPTED:
			if( handle->user_cb[_RADIO_EVENT_TYPE_INTERRUPT] )
			{
				((radio_interrupted_cb)handle->user_cb[_RADIO_EVENT_TYPE_INTERRUPT])(__convert_interrupted_code(msg->code),handle->user_data[_RADIO_EVENT_TYPE_INTERRUPT]);
			}
			break;
		case MM_MESSAGE_READY_TO_RESUME:
			if( handle->user_cb[_RADIO_EVENT_TYPE_INTERRUPT] )
			{
				((radio_interrupted_cb)handle->user_cb[_RADIO_EVENT_TYPE_INTERRUPT])(RADIO_INTERRUPTED_COMPLETED,handle->user_data[_RADIO_EVENT_TYPE_INTERRUPT]);
			}
			break;
		case  MM_MESSAGE_ERROR:
				__convert_error_code(msg->code,(char*)__FUNCTION__);
			break;
		case MM_MESSAGE_RADIO_SCAN_START:
			LOGI("[%s] Scan Started");
			break;
		case  MM_MESSAGE_STATE_CHANGED:
			handle->state = __convert_radio_state(msg->state.current);
			LOGI("[%s] State Changed --- from : %d , to : %d" ,__FUNCTION__,  __convert_radio_state(msg->state.previous), handle->state);
			break;
		case MM_MESSAGE_RADIO_SEEK_START:
			LOGI("[%s] Seek Started", __FUNCTION__);
			break;
		default:
			break;
	}
	return 1;
}

static int __radio_check_system_info_feature_supported()
{
	bool bValue = false;
	int nRetVal = false;

	nRetVal = system_info_get_platform_bool("http://tizen.org/feature/fmradio", &bValue);

	if ( nRetVal != SYSTEM_INFO_ERROR_NONE )
	{
		LOGE("[%s] SYSTEM_INFO_ERROR : ", __FUNCTION__);
		return false;
	}

	if ( false == bValue )
	{
		LOGI("system_info_get_platform_bool returned Unsupported feature capability\n");
	}
	else
	{
		LOGI("system_info_get_platform_bool returned Supported status feature\n");
	}

	return bValue;
}

/*
* Public Implementation
*/
int radio_create(radio_h *radio)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle;

	handle = (radio_s*)malloc( sizeof(radio_s));
	if (handle != NULL)
		memset(handle, 0 , sizeof(radio_s));
	else
	{
		LOGE("[%s] RADIO_ERROR_OUT_OF_MEMORY(0x%08x)" ,__FUNCTION__,RADIO_ERROR_OUT_OF_MEMORY);
		return RADIO_ERROR_OUT_OF_MEMORY;
	}
	int ret = mm_radio_create(&handle->mm_handle);
	if( ret != MM_ERROR_NONE)
	{
		free(handle);
		handle=NULL;
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		*radio = (radio_h)handle;

		ret = mm_radio_set_message_callback(handle->mm_handle, __msg_callback, (void*)handle);
		if(ret != MM_ERROR_NONE)
		{
			LOGW("[%s] Failed to set message callback function (0x%x)" ,__FUNCTION__, ret);
		}
		ret = mm_radio_realize(handle->mm_handle);
		if(ret != MM_ERROR_NONE)
		{
			return __convert_error_code(ret,(char*)__FUNCTION__);
		}
		handle->state = RADIO_STATE_READY;
		handle->mute = FALSE;
		return RADIO_ERROR_NONE;
	}
}

int radio_destroy(radio_h radio)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;

	int ret;
	ret = mm_radio_unrealize(handle->mm_handle);
	if ( ret!= MM_ERROR_NONE)
	{
		LOGW("[%s] Failed to unrealize (0x%x)" ,__FUNCTION__, ret);
	}

	ret = mm_radio_destroy(handle->mm_handle);
	if (ret!= MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		free(handle);
		handle= NULL;
		return RADIO_ERROR_NONE;
	}
}

int  radio_get_state(radio_h radio, radio_state_e *state)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	RADIO_NULL_ARG_CHECK(state);
	radio_s * handle = (radio_s *) radio;
	MMRadioStateType currentStat = MM_RADIO_STATE_NULL;
	int ret = mm_radio_get_state(handle->mm_handle, &currentStat);
	if(ret != MM_ERROR_NONE)
	{
		*state = handle->state;
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		handle->state  = __convert_radio_state(currentStat);
		*state = handle->state;
		return RADIO_ERROR_NONE;
	}
}

int radio_start(radio_h radio)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;
	RADIO_STATE_CHECK(handle,RADIO_STATE_READY);

	int ret = mm_radio_start(handle->mm_handle);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		handle->state = RADIO_STATE_PLAYING;
		return RADIO_ERROR_NONE;
	}
}

int radio_stop(radio_h radio)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;
	RADIO_STATE_CHECK(handle,RADIO_STATE_PLAYING);

	int ret = mm_radio_stop(handle->mm_handle);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		handle->state = RADIO_STATE_READY;
		return RADIO_ERROR_NONE;
	}
}

int radio_seek_up(radio_h radio,radio_seek_completed_cb callback, void *user_data )
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;
	RADIO_STATE_CHECK(handle,RADIO_STATE_PLAYING);

	if(callback!=NULL)
	{
		__set_callback(_RADIO_EVENT_TYPE_SEEK_FINISH,radio,callback,user_data);
	}
	else
	{
		__unset_callback(_RADIO_EVENT_TYPE_SEEK_FINISH,radio);
	}

	int ret = mm_radio_seek(handle->mm_handle, MM_RADIO_SEEK_UP);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		return RADIO_ERROR_NONE;
	}
}

int radio_seek_down(radio_h radio,radio_seek_completed_cb callback, void *user_data )
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;
	RADIO_STATE_CHECK(handle,RADIO_STATE_PLAYING);

	if(callback!=NULL)
	{
		__set_callback(_RADIO_EVENT_TYPE_SEEK_FINISH,radio,callback,user_data);
	}
	else
	{
		__unset_callback(_RADIO_EVENT_TYPE_SEEK_FINISH,radio);
	}

	int ret = mm_radio_seek(handle->mm_handle, MM_RADIO_SEEK_DOWN);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		return RADIO_ERROR_NONE;
	}
}

int radio_set_frequency(radio_h radio, int frequency)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	if(frequency < 87500 || frequency > 108000)
	{
		LOGE("[%s] RADIO_ERROR_INVALID_PARAMETER(0x%08x) : Out of range (87500 ~ 108000)" ,__FUNCTION__,RADIO_ERROR_INVALID_PARAMETER);
		return RADIO_ERROR_INVALID_PARAMETER;
	}
	int freq= frequency;
	radio_s * handle = (radio_s *) radio;
	int ret = mm_radio_set_frequency(handle->mm_handle, freq);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		return RADIO_ERROR_NONE;
	}
}

int radio_get_frequency(radio_h radio, int *frequency)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	RADIO_NULL_ARG_CHECK(frequency);
	radio_s * handle = (radio_s *) radio;

	int freq;
	int ret = mm_radio_get_frequency(handle->mm_handle, &freq);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		*frequency = freq;
		return RADIO_ERROR_NONE;
	}
}

int radio_get_signal_strength(radio_h radio, int *strength)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	RADIO_NULL_ARG_CHECK(strength);
	radio_s * handle = (radio_s *) radio;

	int _strength;
	int ret = mm_radio_get_signal_strength(handle->mm_handle, &_strength);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		*strength = _strength;
		return RADIO_ERROR_NONE;
	}
}

int radio_scan_start(radio_h radio, radio_scan_updated_cb callback, void *user_data)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;
	RADIO_STATE_CHECK(handle,RADIO_STATE_READY);

	if(callback!=NULL)
	{
		__set_callback(_RADIO_EVENT_TYPE_SCAN_INFO,radio,callback,user_data);
	}
	else
	{
		__unset_callback(_RADIO_EVENT_TYPE_SCAN_INFO,radio);
	}

	int ret = mm_radio_scan_start(handle->mm_handle);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		handle->state = RADIO_STATE_SCANNING;
		return RADIO_ERROR_NONE;
	}
}

int radio_scan_stop(radio_h radio, radio_scan_stopped_cb callback, void *user_data)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;
	RADIO_STATE_CHECK(handle,RADIO_STATE_SCANNING);

	if(callback!=NULL)
	{
		__set_callback(_RADIO_EVENT_TYPE_SCAN_STOP,radio,callback,user_data);
	}
	else
	{
		__unset_callback(_RADIO_EVENT_TYPE_SCAN_STOP,radio);
	}

	int ret = mm_radio_scan_stop(handle->mm_handle);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		handle->state = RADIO_STATE_READY;
		return RADIO_ERROR_NONE;
	}
}


int radio_set_mute(radio_h radio, bool muted)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	radio_s * handle = (radio_s *) radio;

	int ret = mm_radio_set_mute(handle->mm_handle, muted);
	if(ret != MM_ERROR_NONE)
	{
		return __convert_error_code(ret,(char*)__FUNCTION__);
	}
	else
	{
		handle->mute = muted;
		return RADIO_ERROR_NONE;
	}
}

int radio_is_muted(radio_h radio, bool *muted)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	RADIO_INSTANCE_CHECK(radio);
	RADIO_NULL_ARG_CHECK(muted);
	radio_s * handle = (radio_s *) radio;
	*muted = handle->mute;
	return RADIO_ERROR_NONE;
}

int radio_set_scan_completed_cb(radio_h radio, radio_scan_completed_cb callback, void *user_data)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	return __set_callback(_RADIO_EVENT_TYPE_SCAN_FINISH,radio,callback,user_data);
}

int radio_unset_scan_completed_cb(radio_h radio)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	return __unset_callback(_RADIO_EVENT_TYPE_SCAN_FINISH,radio);
}

int radio_set_interrupted_cb(radio_h radio, radio_interrupted_cb callback, void *user_data)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	return __set_callback(_RADIO_EVENT_TYPE_INTERRUPT,radio,callback,user_data);
}

int radio_unset_interrupted_cb(radio_h radio)
{
	LOGI("[%s] Enter", __func__);
	RADIO_SUPPORT_CHECK(__radio_check_system_info_feature_supported());
	return __unset_callback(_RADIO_EVENT_TYPE_INTERRUPT,radio);
}

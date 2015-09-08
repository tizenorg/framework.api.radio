/*
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: JongHyuk Choi <jhchoi.choi@samsung.com>, YoungHwan An <younghwan_.an@samsung.com>
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
 *
 */

/* testsuite for radio api */
#include <stdlib.h>
#include <stdio.h>


#include "radio.h"
#include "radio_test_type.h"
#include <unistd.h>

#define DEFAULT_TEST_FREQ	107700
#define MENU_ITEM_MAX	19
#define _MAX_INPUT_STRING_ 100


/* test items...*/
int __test_radio_init(void);
int __test_radio_listen_gorealra(void);
int __test_repeat_init_release(void);
int __test_repeat_start_stop(void);
int __test_repeat_seek(void);
int __test_repeat_whole(void);
int __test_manual_api_calling(void);
int __test_radio_hw_debug(void);


/* functions*/
static void __print_menu(void);
static void __run_test(int key);

int radio_rt_api_test(void);
static int __menu(void);
static void __call_api( int choosen );
void __radio_seek_completed_cb(int frequency, void *user_data);
void __radio_scan_updated_cb(int freq, void *user_data);
void __radio_scan_stop_cb(void *user_data);
void __radio_set_scan_completed_cb(void *user_param);
void __radio_set_interrupted_cb(radio_interrupted_code_e code, void *user_param);



/* list of tests*/
test_item_t g_tests[100] =
{
	/* menu string : short string to be displayed to menu
	     description : detailed description
	     test function :  a pointer to a actual test function
	     0 : to be filled with return value of test function
	 */
	{
		"init test",
		"check radio init function",
		__test_radio_init,
	0
	},

	{
		"listening gorealra",
		"let's listen to the gorealra!",
		__test_radio_listen_gorealra,
	0
	},

	{
		"repeat_init_release",
		"repeat init and release and check if it working and memory usage increment",
		__test_repeat_init_release,
	0
	},

	{
		"repeat_start_stop",
		"repeat start and stop and check if it working and memory usage increment",
		__test_repeat_start_stop,
	0
	},

	{
		"repeat_seek",
		"repeat seek and check if it working and memory usage increment",
		__test_repeat_seek,
	0
	},

	{
		"repeat_whole",
		"repeat whole radio sequence and check if it working and memory usage increment",
		__test_repeat_whole,
	0
	},

	{
		"manual api calling test",
		"mapping each api to each test manu. just like other testsuite. try to reproduce the bugs with it.",
		__test_manual_api_calling,
	0
	},

	/* add tests here*/

	/* NOTE : do not remove this last item */
	{"end", "", NULL, 0},
};

int g_num_of_tests = 0;
static radio_h g_my_radio = 0;

int main(int argc, char **argv)
{
	int key = 0;

	do {
		__print_menu();

		do {
			key = getchar();

			if ( key >= '0' && key <= '9')
			{
				__run_test( key - '0' );
			}
		}while ( key == '\n' );
		if(key == 'Q' || key == 'q')
			break;
	}while(1);

	printf("radio test client finished\n");

	return 0;
}

void __print_menu(void)
{
	int i = 0;

	printf("\n\nFMRadio testing menu\n");
	printf("------------------------------------------\n");

	for ( i = 0; g_tests[i].func; i++ )
	{
		printf( "[%d] %s\n", i, g_tests[i].menu_string );
	}
	printf("[q] quit\n");

	g_num_of_tests = i;

	printf("Choose one : ");
}

void __run_test(int key)
{
	int ret = 0;

	/* check index */
	printf("#tests : %d    key : %d\n", g_num_of_tests, key);
	if ( key >= g_num_of_tests || key < 0 )
	{
		printf("unassigned key has pressed : %d\n", key);
		return;
	}

	/* display description*/
	printf( "excuting test : %s\n", g_tests[key].menu_string );
	printf( "description : %s\n", g_tests[key].description );

	/* calling test function*/
	ret = g_tests[key].func();

	g_tests[key].result =  ret;

	if ( ret )
	{
		printf( "TEST FAILED. ret code : %d\n", g_tests[key].result);
	}
	else
	{
		printf( "TEST SUCCEDED. ret code : %d\n", g_tests[key].result);
	}
}


/* test items...*/
int __test_radio_init(void)
{
	printf("%s\n", __FUNCTION__);

	int ret = RADIO_ERROR_NONE;
	radio_h radio;

	RADIO_TEST__( radio_create(&radio); )
	RADIO_TEST__( radio_destroy(radio); )
	return ret;
}

int __test_radio_listen_gorealra(void)
{
	printf("%s\n", __FUNCTION__);

	int ret = RADIO_ERROR_NONE;
	radio_h radio;

	RADIO_TEST__( radio_create(&radio); )
	RADIO_TEST__( radio_set_frequency( radio, DEFAULT_TEST_FREQ ); )
	RADIO_TEST__( radio_start(radio); )
	usleep(5000 * 1000);
	RADIO_TEST__( radio_stop(radio); )
	RADIO_TEST__( radio_destroy(radio); )
	return ret;
}

int __test_repeat_init_release(void)
{
	printf("%s\n", __FUNCTION__);

	int ret = RADIO_ERROR_NONE;
	int cnt = 0;
	radio_h radio;

	while ( cnt < 1000 )
	{
		RADIO_TEST__( radio_create(&radio); )
		RADIO_TEST__( radio_destroy(radio); )

		cnt++;

		printf("%s : repeat count : %d\n", __FUNCTION__, cnt);
	}

	return 0;
}

int __test_repeat_start_stop(void)
{
	printf("%s\n", __FUNCTION__);
	int ret = RADIO_ERROR_NONE;
	int cnt = 0;
	radio_h radio;

	RADIO_TEST__( radio_create(&radio); )
	RADIO_TEST__( radio_set_frequency( radio, DEFAULT_TEST_FREQ ); )

	while(cnt < 10)
	{
		RADIO_TEST__( radio_start(radio); )
		usleep(2000 * 1000);
		RADIO_TEST__( radio_stop(radio); )

		cnt++;

		printf("%s : repeat count : %d\n", __FUNCTION__, cnt);
	}

	return 0;
}

int __test_repeat_seek(void)
{
	printf("__test_repeat_seek\n");
	return 0;
}

int __test_repeat_whole(void)
{
	printf("__test_repeat_whole\n");
	return 0;
}

int __test_manual_api_calling(void)
{

	radio_rt_api_test();

	return 0;
}


int radio_rt_api_test(void)
{
	while(1)
	{
		int choosen = 0;

		choosen = __menu();

		if ( choosen == -1)
			continue;

		if ( choosen == 0 )
			break;

		__call_api( choosen );
	}

	printf("radio test client finished\n");

	return 0;
}

int __menu(void)
{
	int menu_item = 0;

	printf("---------------------------------------------------------\n");
	printf("radio rt api test. try now!\n");
	printf("---------------------------------------------------------\n");
	printf("[1] radio_create\n");
	printf("[2] radio_destroy\n");
	printf("[3] radio_get_state\n");
	printf("[4] radio_start\n");
	printf("[5] radio_stop\n");
	printf("[6] radio_seek_up\n");
	printf("[7] radio_seek_down\n");
	printf("[8] radio_set_frequency(ex.107700)\n");
	printf("[9] radio_get_frequency\n");
	printf("[10] radio_signal_strength\n");
	printf("[11] radio_scan_start\n");
	printf("[12] radio_scan_stop\n");
	printf("[13] radio_set_mute\n");
	printf("[14] radio_is_muted\n");
	printf("[15] radio_set_scan_completed_cb\n");
	printf("[16] radio_unset_scan_completed_cb\n");
	printf("[17] radio_set_interrupted_cb\n");
	printf("[18] radio_unset_interrupted_cb\n");

	printf("[0] quit\n");
	printf("---------------------------------------------------------\n");
	printf("choose one : ");

	if ( scanf("%d", &menu_item) == 0)
	{
		char temp[_MAX_INPUT_STRING_];
		if (scanf("%s", temp) ==0)
		{
			printf("Error while flushing the input buffer - but lets continue\n");
		}
		return -1;
	}


	if ( menu_item > MENU_ITEM_MAX )
		menu_item = -1;

	return menu_item;
}

void __call_api( int choosen )
{
	int ret = RADIO_ERROR_NONE;

	switch( choosen )
	{
		case 1:
		{
			RADIO_TEST__( radio_create( &g_my_radio ); )
		}
		break;

		case 2:
		{
			RADIO_TEST__( radio_destroy( g_my_radio ); )
			g_my_radio = 0;
		}
		break;

		case 3:
		{
			radio_state_e state;
			RADIO_TEST__( radio_get_state(g_my_radio, &state); )

			printf("state : %d\n", state);
		}
		break;

		case 4:
		{
			RADIO_TEST__( radio_start(g_my_radio); )
		}
		break;

		case 5:
		{
			RADIO_TEST__( radio_stop(g_my_radio); )
		}
		break;

		case 6:
		{
			RADIO_TEST__( radio_seek_up(g_my_radio, __radio_seek_completed_cb, NULL); )

		}
		break;

		case 7:
		{
			RADIO_TEST__( radio_seek_down(g_my_radio, __radio_seek_completed_cb, NULL); )
		}
		break;

		case 8:
		{
			int freq = 0;
			printf("input freq : ");
			if (scanf("%d", &freq) == 0)
				return;

			RADIO_TEST__( radio_set_frequency(g_my_radio, freq); )
		}
		break;

		case 9:
		{
			int freq = 0;
			RADIO_TEST__( radio_get_frequency(g_my_radio, &freq ); )

			printf("freq : %d\n", freq);
		}
		break;

		case 10:
		{
			int signal_strength = 0;
			RADIO_TEST__( radio_get_signal_strength(g_my_radio, &signal_strength); )
			printf("signal strength is : %d \n", signal_strength);
		}
		break;

		case 11:
		{
			RADIO_TEST__( radio_scan_start(g_my_radio, &__radio_scan_updated_cb, NULL); )
		}
		break;

		case 12:
		{
			RADIO_TEST__( radio_scan_stop(g_my_radio, &__radio_scan_stop_cb, NULL); )
		}
		break;

		case 13:
		{
			int muted = 0;
			printf("select one(0:UNMUTE/1:MUTE) : ");
			if ( scanf("%d", &muted) == 0)
				return;
			RADIO_TEST__( radio_set_mute(g_my_radio, muted); )
		}
		break;

		case 14:
		{
			bool muted = 0;
			RADIO_TEST__( radio_is_muted(g_my_radio, &muted); )
			printf("muted : %d \n", muted);
		}
		break;


		case 15:
		{
			RADIO_TEST__( radio_set_scan_completed_cb(g_my_radio, &__radio_set_scan_completed_cb, NULL); )
		}
		break;

		case 16:
		{
			RADIO_TEST__( radio_unset_scan_completed_cb(g_my_radio); )
		}
		break;

		case 17:
		{
			RADIO_TEST__( radio_set_interrupted_cb(g_my_radio, &__radio_set_interrupted_cb, NULL ); )
		}
		break;

		case 18:
		{
			RADIO_TEST__( radio_unset_interrupted_cb(g_my_radio); )
		}
		break;

		default:
			break;
	}
}


void __radio_seek_completed_cb(int frequency, void *user_data)
{
	printf("__radio_seek_completed_cb freq is %d\n" , frequency);
}
void __radio_scan_updated_cb(int frequency, void *user_param)
{
	printf("__radio_scan_updated_cb freq is %d\n" , frequency);
}

void __radio_scan_stop_cb(void *user_param)
{
	printf("__radio_scan_stop_cb\n");
}

void __radio_set_scan_completed_cb(void *user_param)
{
	printf("__radio_scan_completed_cb\n");
}

void __radio_set_interrupted_cb(radio_interrupted_code_e code, void *user_param)
{
	printf("__radio_set_interrupted_cb\n");
}


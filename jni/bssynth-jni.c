#include <assert.h>
#include <jni.h>
#include <string.h>

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
#include <android/log.h>

#include "bsmd.h"

typedef struct
{
	JNIEnv* env;
	jobject thiz;
} JNI_INFOMATION;

static BSMD_HANDLE handle = NULL;
static BSMD_FUNC *api = NULL;

int Java_jp_bismark_bssynth_App_Initialize
	(
	JNIEnv* env,
	jobject thiz,
	jstring libraryPath,
	jint sampleRate,
	jint blockSize
	)
{
	BSMD_ERR err = BSMD_OK;

	__android_log_print (ANDROID_LOG_INFO, "JNI", "Initialize (%d, %d)", sampleRate, blockSize);

	api = bsmdLoad ();

	if (err == BSMD_OK) {
		// initialize
		const unsigned char key[64] = {
      // This keycode has expire date (2014/07/31)
      // Do not use this code for distribution
      0xA9, 0x89, 0x5F, 0x31, 0xDF, 0x5E, 0x40, 0xE5,
      0x65, 0xE9, 0xB3, 0x9C, 0x53, 0x45, 0x64, 0xFF,
      0xA1, 0x16, 0x30, 0x0F, 0xDF, 0xBE, 0x1B, 0x37,
      0xCE, 0x40, 0x7C, 0xA3, 0xF6, 0xF8, 0x36, 0xD5,
      0xC0, 0xB2, 0x56, 0x62, 0x03, 0xA0, 0x21, 0x12,
      0xCD, 0xD8, 0x73, 0x12, 0x6C, 0x2F, 0xAB, 0xC7,
      0xB2, 0xA6, 0x06, 0x80, 0x5A, 0x40, 0x16, 0xAE,
      0x38, 0xAC, 0x26, 0xC9, 0x1F, 0xDC, 0x25, 0x52,
		};
		const char *library = (*env)->GetStringUTFChars (env, libraryPath, NULL);
		JNI_INFOMATION JNIInfo = {env, thiz}; /* required */
		err = api->initializeWithSoundLib (&handle, NULL, NULL, library, &JNIInfo, key);
		__android_log_print (ANDROID_LOG_INFO, "JNI", "Load Sound Library (%s)", library);
		(*env)->ReleaseStringUTFChars (env, libraryPath, library);
	}

	if (err == BSMD_OK) {
		unsigned long value = (unsigned long) sampleRate;
		err = api->ctrl (handle, BSMD_CTRL_SET_SAMPLE_RATE, &value, sizeof (value));
	}

	if (err == BSMD_OK) {
		long value = (long) blockSize;
		err = api->ctrl (handle, BSMD_CTRL_SET_BLOCK_SIZE, &value, sizeof (value));
	}

	if (err == BSMD_OK) {
		int buffers = 2;
		api->ctrl (handle, BSMD_CTRL_SET_BUFFERS, &buffers, sizeof (buffers));
	}

	if (err == BSMD_OK) {
		int value = 512;
		api->ctrl (handle, BSMD_CTRL_SET_NUMBER_OF_REGIONS, &value, sizeof (value));
	}

	if (err == BSMD_OK) {
		// open wave output device
		err = api->open (handle, NULL, NULL);
	}

	return err;
}

void Java_jp_bismark_bssynth_App_Finalize (JNIEnv* env, jobject thiz)
{
	__android_log_print (ANDROID_LOG_INFO, "JNI", "Finalize");
	if (api && handle) {
		api->stop (handle);
		api->exit (handle);
	}
	api = NULL;
	handle = NULL;
}

int Java_jp_bismark_bssynth_App_LoadLibrary
	(
	JNIEnv* env,
	jobject thiz,
	jint index,
	jstring libraryPath
	)
{
	BSMD_ERR err = BSMD_OK;

	const char *library = (*env)->GetStringUTFChars (env, libraryPath, NULL);
	__android_log_print (ANDROID_LOG_INFO, "JNI", "Load Sound Library #%d (%s)", index, library);

	if (err == BSMD_OK) {
		BSMD_SOUND_LIBRARY value;
		value.index = index;
		value.path = library;
		err = api->ctrl (handle, BSMD_CTRL_SET_SOUND_LIBRARY, &value, sizeof (value));
	}

	return err;
}

int Java_jp_bismark_bssynth_App_Start(JNIEnv* env, jobject thiz)
{
	if (api && handle) {
		BSMD_ERR err = BSMD_OK;

		if (err == BSMD_OK) {
			if (api->isPlaying (handle) == 0) {
				__android_log_print (ANDROID_LOG_INFO, "JNI", "Start");
				err = api->start (handle);
			}
			else {
				__android_log_print (ANDROID_LOG_INFO, "JNI", "Already Started");
			}
		}

		return err;
	}

	return -1;
}

int Java_jp_bismark_bssynth_App_Stop(JNIEnv* env, jobject thiz)
{
	if (api && handle) {
		BSMD_ERR err = BSMD_OK;

		if (err == BSMD_OK) {
			if (api->isPlaying (handle)) {
				__android_log_print (ANDROID_LOG_INFO, "JNI", "Stop");
				err = api->stop (handle);
			}
			else {
				__android_log_print (ANDROID_LOG_INFO, "JNI", "Already Stopped");
			}
		}

		return err;
	}

	return -1;
}

int Java_jp_bismark_bssynth_App_IsPlaying(JNIEnv* env, jobject thiz)
{
	if (api && handle) {
		return api->isPlaying (handle);
	}
	else return 0;
}

void Java_jp_bismark_bssynth_App_SetChannelMessage (JNIEnv* env, jobject thiz, jint status, jint data1, jint data2)
{
	int port = 0;
	api->setChannelMessage (handle, port, status, data1, data2);
}

jstring Java_jp_bismark_bssynth_App_GetInstrumentName (JNIEnv* env, jobject thiz, jint part)
{
	int module = 0;
	char name[64];
	api->ctrl (handle, (BSMD_CTRL) (BSMD_CTRL_GET_INSTRUMENT_NAME + module * 16 + part), name, sizeof (name));
	return (*env)->NewStringUTF (env, name);
}

void Java_jp_bismark_bssynth_App_SetReverb (JNIEnv* env, jobject thiz, jint value)
{
	int v = value;
	api->ctrl (handle, BSMD_CTRL_SET_REVERB, &v, sizeof (v));
}

void Java_jp_bismark_bssynth_App_SetChorus (JNIEnv* env, jobject thiz, jint value)
{
	int v = value;
	api->ctrl (handle, BSMD_CTRL_SET_CHORUS, &v, sizeof (v));
}

jint Java_jp_bismark_bssynth_App_GetMute (JNIEnv* env, jobject thiz, jint port, jint channel)
{
	int status = 0;
	api->ctrl (handle, (BSMD_CTRL) (BSMD_CTRL_GET_MUTE + port * 16 + channel), &status, sizeof (status));
	return status;
}

void Java_jp_bismark_bssynth_App_SetMute (JNIEnv* env, jobject thiz, jint port, jint channel, jint value)
{
	int status = value;
	api->ctrl (handle, (BSMD_CTRL) (BSMD_CTRL_SET_MUTE + port * 16 + channel), &status, sizeof (status));
}

jint Java_jp_bismark_bssynth_App_GetSolo (JNIEnv* env, jobject thiz, jint port, jint channel)
{
	int status = 0;
	api->ctrl (handle, (BSMD_CTRL) (BSMD_CTRL_GET_SOLO + port * 16 + channel), &status, sizeof (status));
	return status;
}

void Java_jp_bismark_bssynth_App_SetSolo (JNIEnv* env, jobject thiz, jint port, jint channel, jint value)
{
	int status = value;
	api->ctrl (handle, (BSMD_CTRL) (BSMD_CTRL_SET_SOLO + port * 16 + channel), &status, sizeof (status));
}

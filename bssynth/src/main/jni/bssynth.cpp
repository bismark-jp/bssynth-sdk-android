//
// Copyright (c) 2020 bismark LLC All rights reserved
//

#include <assert.h>
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include "bsse.h"
#include "bsmp.h"
#include "bssynth_key.h"

typedef struct
{
    JNIEnv* env;
    jobject thiz;
    jobject context;
} JNI_INFOMATION;

static BSMP_HANDLE handle = NULL;
static BSMP_FUNC *api = NULL;

static int clockCounter;
static int clocks;
static int beats;
static int measures;
static int numerator;
static int denominator;
static int metronomeClick;
static float tempo;
static float timePerClock;
static float currentTime;
static float totalTime;

static char *libraryContent = NULL;
static char *fileContent = NULL;

static void callback (BSMP_HANDLE handle, BSMP_CALLBACK_TYPE type, void *data, void *user)
{
    switch (type) {
        case BSMP_CALLBACK_TYPE_START:
            __android_log_print (ANDROID_LOG_INFO, "JNI", "Started");
            break;
        case BSMP_CALLBACK_TYPE_STOP:
            __android_log_print (ANDROID_LOG_INFO, "JNI", "Stopped");
            break;
        case BSMP_CALLBACK_TYPE_SEEK:
            __android_log_print (ANDROID_LOG_INFO, "JNI", "Seeked");
            clocks = clockCounter = beats = measures = 0;
            numerator = 4;
            denominator = 4;
            metronomeClick = 24;
            tempo = 120.f;
            timePerClock = 500000.f / (24.f * 1000000.f);
            currentTime = 0.f;
            break;

        case BSMP_CALLBACK_TYPE_CLOCK:
            clocks++;
            if ((++clockCounter % metronomeClick) == 0) {
                clockCounter = 0;
                if (numerator <= ++beats) {
                    beats = 0;
                    measures++;
                }
                __android_log_print (ANDROID_LOG_INFO, "JNI", "beat = %03d|%02d (%d clocks)", measures + 1, beats + 1, clocks);
            }
            currentTime += timePerClock;
            break;
        case BSMP_CALLBACK_TYPE_TEMPO:
            {
#if __LP64__
                unsigned long param = *(unsigned int *) data;
#else
                unsigned long param = *(unsigned long *) data;
#endif
                __android_log_print(ANDROID_LOG_INFO, "JNI", "tempo = %lu", param);
                tempo = 60 * 1000 * 1000 / (float) param;
                timePerClock = (float) param / (24.f * 1000000.f);
            }
            break;
        case BSMP_CALLBACK_TYPE_TIME_SIGNATURE:
            {
#if __LP64__
                unsigned long param = *(unsigned int *) data;
#else
                unsigned long param = *(unsigned long *) data;
#endif
                __android_log_print (ANDROID_LOG_INFO, "JNI", "time signature = 0x%08lX", param);
                numerator = (int) ((param >> 24) & 0x000000FF);
                denominator = 1 << (int) ((param >> 16) & 0x000000FF);
                metronomeClick = (int) ((param >> 8) & 0x000000FF);
                clockCounter = 0;
            }
            break;
        default:
            break;
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_initialize (JNIEnv* env, jobject thiz, jobject context, jobject assetManager, jint sampleRate, jint framesPerBuffer, jstring libraryName)
{
    BSMP_ERR err = BSMP_OK;

    api = bsmpLoad ();

    if (err == BSMP_OK) {
        // initialize
        __android_log_print (ANDROID_LOG_INFO, "JNI", "Initialize");
        JNI_INFOMATION JNIInfo = {env, thiz, context};
        err = api->initialize (&handle, callback, NULL, &JNIInfo, bssynth_key);
    }

    if (err == BSMP_OK) {
        // reverb on
        __android_log_print (ANDROID_LOG_INFO, "JNI", "Set reverb = on");
        int value = 1;
        err = api->ctrl (handle, BSMP_CTRL_SET_REVERB, &value, sizeof (value));
    }

    if (err == BSMP_OK) {
        // chorus on
        __android_log_print (ANDROID_LOG_INFO, "JNI", "Set chorus = on");
        int value = 1;
        err = api->ctrl (handle, BSMP_CTRL_SET_CHORUS, &value, sizeof (value));
    }

    if (err == BSMP_OK) {
        // sample rate
        int value = sampleRate;
        __android_log_print (ANDROID_LOG_INFO, "JNI", "Set sample rate to %d", value);
        err = api->ctrl (handle, BSMP_CTRL_SET_SAMPLE_RATE, &value, sizeof (value));
    }

    if (err == BSMP_OK) {
        // block size
        long value = framesPerBuffer;
        __android_log_print (ANDROID_LOG_INFO, "JNI", "Set block size to %ld", value);
        err = api->ctrl (handle, BSMP_CTRL_SET_BLOCK_SIZE, &value, sizeof (value));
    }

    if (err == BSMP_OK) {
        AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
        const char *name = env->GetStringUTFChars (libraryName, 0);
        AAsset* file = AAssetManager_open(mgr, name, AASSET_MODE_BUFFER);
        env->ReleaseStringUTFChars (libraryName, name);
        size_t fileLength = AAsset_getLength(file);

        if (libraryContent) {
            delete [] libraryContent;
            libraryContent = NULL;
        }
        libraryContent = new char[fileLength];

        AAsset_read(file, libraryContent, fileLength);

        BSMP_SOUND_LIBRARY_MEMORY info = {0, libraryContent, fileLength};
        __android_log_print (ANDROID_LOG_INFO, "JNI", "Load library from Assets");
        err = api->ctrl (handle, BSMP_CTRL_SET_SOUND_LIBRARY_MEMORY, &info, sizeof (info));
    }

    if (err == BSMP_OK) {
        // open wave output device
        __android_log_print (ANDROID_LOG_INFO, "JNI", "Open");
        err = api->open (handle, NULL, NULL);
    }

    return (jint) err;
}

extern "C" JNIEXPORT void JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_finalize (JNIEnv* env, jobject thiz)
{
    if (api && handle) {
        api->close (handle);
        api->exit (handle);
    }
    api = NULL;
    handle = NULL;

    if (libraryContent) {
        delete [] libraryContent;
        libraryContent = NULL;
    }

    if (fileContent) {
        delete [] fileContent;
        fileContent = NULL;
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_setFileFromPath (JNIEnv* env, jobject thiz, jstring filePath)
{
    if (api && handle) {
        BSMP_ERR err = BSMP_OK;

        if (err == BSMP_OK) {
            const char *file = env->GetStringUTFChars (filePath, 0);
            __android_log_print (ANDROID_LOG_INFO, "JNI", "SetFile %s", file);
            err = api->setFile (handle, file);
            env->ReleaseStringUTFChars (filePath, file);
        }

        if (err == BSMP_OK) {
            clocks = clockCounter = beats = measures = 0;
            numerator = 4;
            denominator = 4;
            metronomeClick = 24;
            tempo = 120.f;
            timePerClock = 500000.f / (24.f * 1000000.f);
            currentTime = 0.f;

            unsigned long time;
            if (api->getFileInfo (handle, NULL, NULL, NULL, &time) == BSMP_OK) {
                totalTime = (float) time;
            }
        }

        if (err != BSMP_OK) {
            __android_log_print (ANDROID_LOG_INFO, "JNI", "SetFile error = %d", err);
        }

        return err;
    }

    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_setFileFromAssets (JNIEnv* env, jobject thiz, jobject assetManager, jstring fileName)
{
    if (api && handle) {
        BSMP_ERR err = BSMP_OK;

        if (err == BSMP_OK) {
            AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
            const char *name = env->GetStringUTFChars (fileName, 0);
            AAsset* file = AAssetManager_open(mgr, name, AASSET_MODE_BUFFER);
            env->ReleaseStringUTFChars (fileName, name);
            size_t fileLength = AAsset_getLength(file);

            if (fileContent) {
                delete [] fileContent;
                fileContent = NULL;
            }
            fileContent = new char[fileLength];

            AAsset_read(file, fileContent, fileLength);

            __android_log_print (ANDROID_LOG_INFO, "JNI", "SetFileFromAssets");
            err = api->setFileMemory(handle, fileContent, fileLength);
        }

        if (err == BSMP_OK) {
            clocks = clockCounter = beats = measures = 0;
            numerator = 4;
            denominator = 4;
            metronomeClick = 24;
            tempo = 120.f;
            timePerClock = 500000.f / (24.f * 1000000.f);
            currentTime = 0.f;

            unsigned long time;
            if (api->getFileInfo (handle, NULL, NULL, NULL, &time) == BSMP_OK) {
                totalTime = (float) time;
            }
        }

        if (err != BSMP_OK) {
            __android_log_print (ANDROID_LOG_INFO, "JNI", "SetFile error = %d", err);
        }

        return err;
    }

    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_isPlaying (JNIEnv* env, jobject thiz)
{
    if (api && handle) {
        return api->isPlaying (handle);
    }

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_start (JNIEnv* env, jobject thiz)
{
    __android_log_print (ANDROID_LOG_INFO, "JNI", "Start");

    if (api && handle) {
        BSMP_ERR err = BSMP_OK;

        if (err == BSMP_OK) {
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

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_stop (JNIEnv* env, jobject thiz)
{
    __android_log_print (ANDROID_LOG_INFO, "JNI", "Stop");

    if (api && handle) {
        BSMP_ERR err = BSMP_OK;

        if (err == BSMP_OK) {
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

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_seek (JNIEnv* env, jobject thiz, int clock)
{
    __android_log_print (ANDROID_LOG_INFO, "JNI", "Seek (%d[clocks]", clock);

    unsigned long value = 0UL;
    unsigned short division;
    api->getFileInfo (handle, NULL, &division, NULL, NULL);
    if (division > 0) value = clock * division / 24;

    clocks = clockCounter = beats = measures = 0;
    numerator = 4;
    denominator = 4;
    metronomeClick = 24;
    tempo = 120.f;
    timePerClock = 500000.f / (24.f * 1000000.f);
    currentTime = 0.f;

    api->seek (handle, value);

    return 1;
}

static int callback_bounce (int percent, void *user) {
    __android_log_print (ANDROID_LOG_INFO, "JNI", "Bounce (%d%%)", percent);
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_bounce (JNIEnv* env, jobject thiz, jstring path, jint type)
{
    const char *file = env->GetStringUTFChars (path, 0);
    __android_log_print (ANDROID_LOG_INFO, "JNI", "Bounce (%s)", file);

    if (api && handle) {
        BSMP_ERR err = BSMP_OK;

        if (err == BSMP_OK) {
            err = api->bounce (handle, file, (BSMP_WAVE_FILE) type, callback_bounce, NULL);
        }

        return err;
    }

    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getCurrentClocks (JNIEnv* env, jobject thiz)
{
    return clocks;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getTotalClocks (JNIEnv* env, jobject thiz)
{
    unsigned short division;
    unsigned long value = 0UL;

    api->getFileInfo (handle, NULL, &division, &value, NULL);
    if (division > 0) value = value * 24 / division;

    return (int) value;
}

extern "C" JNIEXPORT void JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_setKeyControl (JNIEnv* env, jobject thiz, jint value)
{
    if (api && handle) {
        int v = value;
        api->ctrl (handle, BSMP_CTRL_SET_MASTER_KEY, &v, sizeof (int));
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getKeyControl (JNIEnv* env, jobject thiz)
{
    if (api && handle) {
        int v;
        api->ctrl (handle, BSMP_CTRL_GET_MASTER_KEY, &v, sizeof (int));
        return (jint) v;
    }

    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_setSpeedControl (JNIEnv* env, jobject thiz, jint value)
{
    if (api && handle) {
        int v = value;
        api->ctrl (handle, BSMP_CTRL_SET_SPEED, &v, sizeof (int));
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getSpeedControl (JNIEnv* env, jobject thiz)
{
    if (api && handle) {
        int v;
        api->ctrl (handle, BSMP_CTRL_GET_SPEED, &v, sizeof (int));
        return (jint) v;
    }

    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_setMute (JNIEnv* env, jobject thiz, jint module, jint part, jint value)
{
    if (api && handle) {
        int v = value;
        api->ctrl (handle, (BSMP_CTRL) (BSMP_CTRL_SET_MUTE + module * 16 + part), &v, sizeof (int));
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getMute (JNIEnv* env, jobject thiz, jint module, jint part)
{
    if (api && handle) {
        int v;
        api->ctrl (handle, (BSMP_CTRL) (BSMP_CTRL_GET_MUTE + module * 16 + part), &v, sizeof (int));
        return (jint) v;
    }

    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_setSolo (JNIEnv* env, jobject thiz, jint module, jint part, jint value)
{
    if (api && handle) {
        int v = value;
        api->ctrl (handle, (BSMP_CTRL) (BSMP_CTRL_SET_SOLO + module * 16 + part), &v, sizeof (int));
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getSolo (JNIEnv* env, jobject thiz, jint module, jint part)
{
    if (api && handle) {
        int v;
        api->ctrl (handle, (BSMP_CTRL) (BSMP_CTRL_GET_SOLO + module * 16 + part), &v, sizeof (int));
        return (jint) v;
    }

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getControlChangeMessage (JNIEnv* env, jobject thiz, jint module, jint part, jint control, jint value)
{
    if (api && handle) {
        int v = value;
        return (jint) api->getControlChangeMessage(handle, module, part, control);
    }

    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getProgramChangeMessage (JNIEnv* env, jobject thiz, jint module, jint part, jint value)
{
    if (api && handle) {
        int v = value;
        return (jint) api->getProgramChangeMessage(handle, module, part);
    }

    return -1;
}

extern "C" JNIEXPORT void JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_insertChannelMessage (JNIEnv* env, jobject thiz, jint port, jint status, jint data1, jint data2)
{
    if (api && handle) {
        api->insertChannelMessage(handle, (unsigned char) port, (unsigned char) status, (unsigned char) data1, (unsigned char) data2);
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getUseForRhythmPart (JNIEnv* env, jobject thiz, jint module, jint part)
{
    if (api && handle) {
        return (jint) api->getUseForRhythmPart(handle, module, part);
    }

    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getLibraryNumInstruments (JNIEnv* env, jobject thiz, jint libraryIndex)
{
    if (api && handle) {
        BSSE_FUNC *engine_;
        BSSE_HANDLE handle_;
        api->ctrl (handle, BSMP_CTRL_GET_BSSE_FUNC, &engine_, sizeof (BSSE_FUNC *));
        api->ctrl (handle, BSMP_CTRL_GET_BSSE_HANDLE, &handle_, sizeof (BSSE_HANDLE));
        return (jint) engine_->getLibraryNumPreset (handle_, libraryIndex);
    }

    return -1;
}

extern "C" JNIEXPORT jstring JNICALL
Java_jp_bismark_bssynth_BssynthPlayer_getInstrument(JNIEnv* env, jobject thiz, jint libraryIndex, jint instrumentIndex) {
    const char *result = "";
    char properties[256];

    if (api && handle) {
        BSSE_FUNC *engine_;
        BSSE_HANDLE handle_;
        api->ctrl (handle, BSMP_CTRL_GET_BSSE_FUNC, &engine_, sizeof (BSSE_FUNC *));
        api->ctrl (handle, BSMP_CTRL_GET_BSSE_HANDLE, &handle_, sizeof (BSSE_HANDLE));

        unsigned char bankMsb = 0;
        unsigned char bankLsb = 0;
        unsigned char program = 0;
        int isDrum = 0;
        char name[128];
        engine_->getLibraryPresetProperties (handle_, (int) libraryIndex, (int) instrumentIndex, &bankMsb, &bankLsb, &program, &isDrum, name, sizeof (name) - 1);

        sprintf(properties, "%d,%d,%d,%d,%s", (int) isDrum, (int) bankMsb, (int) bankLsb, (int) program, name);
        // __android_log_print (ANDROID_LOG_INFO, "JNI", "%s\n", properties);
        result = properties;
    }

    return env->NewStringUTF(result);
}

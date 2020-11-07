/* bsse.h - bssynth Synthesizer Engine Library*/

/* Copyright (c) 2002 - 2019 bismark LLC. All rights reserved. */

/*
modification history
--------------------
*/

/* 
DESCRIPTION 
*/

#ifndef __INCbsseh
#define __INCbsseh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

/* defines */

/* BSSE_EXPORTS should be UNDEFINED by applications using bsse.dll */

#ifdef BSSE_STATIC
	#define BSSE_API
/*	#define BSSE_API */
#else /* BSSE_STATIC */
	#ifdef BSSE_EXPORTS
		#ifdef _MSC_VER
			#define BSSE_API __declspec(dllexport)
		#else /* _MSC_VER */
			#define BSSE_API 
		#endif /* _MSC_VER */
	#else /* BSSE_EXPORTS */
		#ifdef _MSC_VER
		#define BSSE_API __declspec(dllimport)
		#else /* _MSC_VER */
			#define BSSE_API 
		#endif /* _MSC_VER */
	#endif /* BSSE_EXPORTS */
#endif /* BSSE_STATIC */

#ifndef _MSC_VER
	#ifndef __TCHAR_DEFINED
		typedef char TCHAR;
		typedef char *LPSTR;
		typedef TCHAR * LPTSTR;
		typedef const TCHAR *LPCTSTR;
		#define __TCHAR_DEFINED
	#endif
#endif /* _MSC_VER */

typedef enum {
	/* errors */
	BSSE_OK = 0,
	BSSE_ERR_PROTECTION = -1,
	BSSE_ERR_INVALID_HANDLE = -2,
	BSSE_ERR_FILE = -10,
	BSSE_ERR_MEMORY = -20,
	BSSE_ERR_RESOURCE = -30,
	BSSE_ERR_PARAM = -40,
	BSSE_ERR_AUDIO_DRIVER = -100,
	BSSE_ERR_DATA = -200,
	BSSE_ERR_MODULE = -800,
	BSSE_ERR_NOT_SUPPORTED = -900,
	BSSE_ERR_UNDEFINED = -901,
} BSSE_ERR;

typedef enum {
	/* common controls: 00000 - 09999 */
	BSSE_CTRL_SET_MASTER_VOLUME = 0, /* master volume */
	BSSE_CTRL_GET_MASTER_VOLUME,
	BSSE_CTRL_SET_MASTER_PANPOT, /* master panpot */
	BSSE_CTRL_GET_MASTER_PANPOT,
	BSSE_CTRL_SET_MASTER_KEY, /* master key */
	BSSE_CTRL_GET_MASTER_KEY,
	BSSE_CTRL_SET_MASTER_TUNE, /* master tune */
	BSSE_CTRL_GET_MASTER_TUNE,

	BSSE_CTRL_SET_REVERB = 100, /* reverb */
	BSSE_CTRL_GET_REVERB,
	BSSE_CTRL_GET_REVERB_AVAILABLE,
	BSSE_CTRL_SET_REVERB_MACRO,
	BSSE_CTRL_GET_REVERB_MACRO,
	BSSE_CTRL_SET_REVERB_CHARACTER,
	BSSE_CTRL_GET_REVERB_CHARACTER,
	BSSE_CTRL_SET_REVERB_PRE_LPF,
	BSSE_CTRL_GET_REVERB_PRE_LPF,
	BSSE_CTRL_SET_REVERB_LEVEL,
	BSSE_CTRL_GET_REVERB_LEVEL,
	BSSE_CTRL_SET_REVERB_TIME,
	BSSE_CTRL_GET_REVERB_TIME,
	BSSE_CTRL_SET_REVERB_DELAY_FEEDBACK,
	BSSE_CTRL_GET_REVERB_DELAY_FEEDBACK,
	BSSE_CTRL_SET_REVERB_PREDELAY_TIME,
	BSSE_CTRL_GET_REVERB_PREDELAY_TIME,

	BSSE_CTRL_SET_CHORUS = 120, /* chorus */
	BSSE_CTRL_GET_CHORUS,
	BSSE_CTRL_GET_CHORUS_AVAILABLE,
	BSSE_CTRL_SET_CHORUS_MACRO,
	BSSE_CTRL_GET_CHORUS_MACRO,
	BSSE_CTRL_SET_CHORUS_PRE_LPF,
	BSSE_CTRL_GET_CHORUS_PRE_LPF,
	BSSE_CTRL_SET_CHORUS_LEVEL,
	BSSE_CTRL_GET_CHORUS_LEVEL,
	BSSE_CTRL_SET_CHORUS_FEEDBACK,
	BSSE_CTRL_GET_CHORUS_FEEDBACK,
	BSSE_CTRL_SET_CHORUS_DELAY,
	BSSE_CTRL_GET_CHORUS_DELAY,
	BSSE_CTRL_SET_CHORUS_RATE,
	BSSE_CTRL_GET_CHORUS_RATE,
	BSSE_CTRL_SET_CHORUS_DEPTH,
	BSSE_CTRL_GET_CHORUS_DEPTH,

	BSSE_CTRL_SET_DELAY = 140, /* delay */
	BSSE_CTRL_GET_DELAY,
	BSSE_CTRL_GET_DELAY_AVAILABLE,
	BSSE_CTRL_SET_DELAY_MACRO,
	BSSE_CTRL_GET_DELAY_MACRO,
	BSSE_CTRL_SET_DELAY_PRE_LPF,
	BSSE_CTRL_GET_DELAY_PRE_LPF,
	BSSE_CTRL_SET_DELAY_TIME,
	BSSE_CTRL_GET_DELAY_TIME,
	BSSE_CTRL_SET_DELAY_LEVEL,
	BSSE_CTRL_GET_DELAY_LEVEL,
	BSSE_CTRL_SET_DELAY_FEEDBACK,
	BSSE_CTRL_GET_DELAY_FEEDBACK,

	BSSE_CTRL_SET_SAMPLE_RATE = 200,
	BSSE_CTRL_GET_SAMPLE_RATE,
	BSSE_CTRL_SET_BLOCK_SIZE,
	BSSE_CTRL_GET_BLOCK_SIZE,
	BSSE_CTRL_SET_CHANNELS,
	BSSE_CTRL_GET_CHANNELS,
	BSSE_CTRL_SET_POLY = 210,
	BSSE_CTRL_GET_POLY,

	/* bsse controls: 10000 - 19999 */
	BSSE_CTRL_GET_FREE_VOICES = 10000,
	BSSE_CTRL_GET_PLAY_VOICES,
	BSSE_CTRL_GET_RELEASE_VOICES,
	BSSE_CTRL_GET_USED_VOICES,

	BSSE_CTRL_SET_NO_INSTRUMENT_FIX = 10010,
	BSSE_CTRL_GET_NO_INSTRUMENT_FIX,
	
	BSSE_CTRL_SET_USE_BREATH_CONTROL_AS_EXPRESSION,

	BSSE_CTRL_SET_NUMBER_OF_REGIONS = 10020,
	
	BSSE_CTRL_SET_BIT_SHIFT_ORDER = 10050, /* <non-floating engine only> */
	BSSE_CTRL_GET_BIT_SHIFT_ORDER, /* <non-floating engine only> */
	BSSE_CTRL_SET_TOTAL_GAIN, /* <floating engine only> */
	BSSE_CTRL_GET_TOTAL_GAIN, /* <floating engine only> */

	BSSE_CTRL_GET_LICENSE_NAME,
	BSSE_CTRL_GET_LICENSE_PERIOD,

	BSSE_CTRL_CLEAR_BUFFER = 10060,
  
	BSSE_CTRL_GET_SOUND_LIBRARY_NUM = 11000,
	BSSE_CTRL_SET_SOUND_LIBRARY = BSSE_CTRL_GET_SOUND_LIBRARY_NUM + 10,
	BSSE_CTRL_SET_SOUND_LIBRARY_MEMORY = BSSE_CTRL_GET_SOUND_LIBRARY_NUM + 20,
	BSSE_CTRL_GET_SOUND_LIBRARY_MEMORY,
	BSSE_CTRL_SET_SOUND_LIBRARY_SEL = BSSE_CTRL_GET_SOUND_LIBRARY_NUM + 30,
	BSSE_CTRL_GET_SOUND_LIBRARY_SEL,

	BSSE_CTRL_GET_INSTRUMENT_NAME = 12000,
	BSSE_CTRL_SET_MUTE = 12100,
	BSSE_CTRL_GET_MUTE = 12200,
	BSSE_CTRL_SET_SOLO = 12300,
	BSSE_CTRL_GET_SOLO = 12400,
} BSSE_CTRL;

typedef enum {
	BSSE_LIBRARY_INDEX_MODE_NORAMAL = 0,
} BSSE_LIBRARY_INDEX_MODE;

/* typedefs */

typedef void *BSSE_HANDLE; 

typedef unsigned char (*BSSE_CALLBACK_VOLUME) (unsigned char module, unsigned char part, unsigned char value, void *user);
typedef unsigned char (*BSSE_CALLBACK_KEY) (unsigned char module, unsigned char part, unsigned char value, void *user);
typedef short (*BSSE_CALLBACK_TUNE) (unsigned char module, unsigned char part, short value, void *user);

typedef struct {
	int index;
	LPCTSTR path;
} BSSE_SOUND_LIBRARY;

typedef struct {
	int index;
	char *address;
	unsigned long size;
} BSSE_SOUND_LIBRARY_MEMORY;

typedef struct {
	int module;
	int part;
	int index;
	BSSE_LIBRARY_INDEX_MODE mode;
} BSSE_SOUND_LIBRARY_SEL;

typedef struct 
{
	int size;

	/* status */
	BSSE_ERR (*initialize) (BSSE_HANDLE *handle, const unsigned char *key, void *target);
	BSSE_ERR (*reset) (BSSE_HANDLE handle);
	BSSE_ERR (*exit) (BSSE_HANDLE handle);

	/* library */
	BSSE_ERR (*loadLibrary) (BSSE_HANDLE handle, int index, LPCTSTR path);
	BSSE_ERR (*loadLibraryMemory) (BSSE_HANDLE handle, int index, char *data, unsigned long size);
	BSSE_ERR (*unloadLibrary) (BSSE_HANDLE handle, int index);
	BSSE_ERR (*getLibraryName) (BSSE_HANDLE handle, int index, LPTSTR name, int length);
	BSSE_ERR (*getLibraryPath) (BSSE_HANDLE handle, int index, LPTSTR path, int length);
	int (*getLibraryNumPreset) (BSSE_HANDLE handle, int index);
	BSSE_ERR (*getLibraryPresetProperties) (BSSE_HANDLE handle, int index, int presetIndex, unsigned char *bankM, unsigned char *bankL, unsigned char *program, int *isDrum, LPTSTR name, int length);
	BSSE_ERR (*getLibraryPresetName) (BSSE_HANDLE handle, int index, unsigned char bankM, unsigned char bankL, unsigned char program, int isDrum, LPTSTR name, int length);

	/* midi message */
	void (*setChannelMessage) (BSSE_HANDLE handle, unsigned char port, unsigned char status, unsigned char data1, unsigned char data2);
	void (*setSystemExclusiveMessage) (BSSE_HANDLE handle, unsigned char port, unsigned char status, const unsigned char *data, int size);

	void (*setRxChannel) (BSSE_HANDLE handle, int module, int part, unsigned char channel);
	unsigned char (*getRxChannel) (BSSE_HANDLE handle, int module, int part);
	unsigned char (*getUseForRhythmPart) (BSSE_HANDLE handle, int module, int part);
	unsigned char (*getProgramChangeMessage) (BSSE_HANDLE handle, int module, int part);
	unsigned char (*getControlChangeMessage) (BSSE_HANDLE handle, int module, int part, unsigned char control);
	unsigned char (*getPitchBendSense) (BSSE_HANDLE handle, int module, int part);
	unsigned char (*getMasterCoarseTune) (BSSE_HANDLE handle, int module, int part);
	unsigned short (*getMasterFineTune) (BSSE_HANDLE handle, int module, int part);
	unsigned short (*getPitchBend) (BSSE_HANDLE handle, int module, int part);
	unsigned char (*getMode) (BSSE_HANDLE handle, int module, int part);

	/* audio processing */
	void (*process) (BSSE_HANDLE handle, short *output, long sampleFrames);
	void (*processReplacing) (BSSE_HANDLE handle, short *output, long sampleFrames);
	void (*process32) (BSSE_HANDLE handle, long *output, long sampleFrames);
	void (*processReplacing32) (BSSE_HANDLE handle, long *output, long sampleFrames);
	void (*processFloat) (BSSE_HANDLE handle, float *output, long sampleFrames);
	void (*processReplacingFloat) (BSSE_HANDLE handle, float *output, long sampleFrames);

	/* calback */
	void (*setCallbackVolume) (BSSE_HANDLE, BSSE_CALLBACK_VOLUME func, void *user);
	void (*setCallbackMasterVolume) (BSSE_HANDLE, BSSE_CALLBACK_VOLUME func, void *user);
	void (*setCallbackMasterKeySet) (BSSE_HANDLE, BSSE_CALLBACK_KEY func, void *user);
	void (*setCallbackMasterTuneSet) (BSSE_HANDLE, BSSE_CALLBACK_TUNE func, void *user);

	/* etc */
	BSSE_ERR (*ctrl) (BSSE_HANDLE handle, BSSE_CTRL ctrl, void *data, int size);
	void (*version) (BSSE_HANDLE handle, LPTSTR str, int length);
} BSSE_FUNC;

typedef BSSE_FUNC *(*BSSE_LOAD) (void);

/* function declarations */

BSSE_API BSSE_FUNC *bsseLoad (void);

#ifdef __cplusplus
}
#endif

#endif /* __INCbsseh */


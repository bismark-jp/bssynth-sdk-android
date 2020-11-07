//
// Copyright (c) 2020 bismark LLC All rights reserved
//

package jp.bismark.bssynth

import android.content.Context
import android.content.res.AssetManager

class BssynthPlayer {

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun initialize(context: Context, assetManager: AssetManager, sampleRate: Int, framesPerBuffer: Int, libraryName: String): Int
    external fun finalize()
    external fun setFileFromPath(filePath: String): Int
    external fun setFileFromAssets(assetManager: AssetManager, fileName: String): Int
    external fun start(): Int
    external fun stop(): Int
    external fun seek(clocks: Int): Int
    external fun isPlaying(): Int
    external fun bounce(path: String, type: Int): Int
    external fun getCurrentClocks(): Int
    external fun getTotalClocks(): Int
    external fun setKeyControl(value: Int)
    external fun getKeyControl(): Int
    external fun setSpeedControl(value: Int)
    external fun getSpeedControl(): Int
    external fun setMute(module: Int, part: Int, value: Int)
    external fun getMute(module: Int, part: Int): Int
    external fun setSolo(module: Int, part: Int, value: Int)
    external fun getSolo(module: Int, part: Int): Int
    external fun getControlChangeMessage(module: Int, part: Int, control: Int): Int
    external fun getProgramChangeMessage(module: Int, part: Int): Int
    external fun insertChannelMessage(port: Int, status: Int, data1: Int, data2: Int)
    external fun getUseForRhythmPart(module: Int, part: Int): Int
    external fun getLibraryNumInstruments(libraryIndex: Int): Int
    external fun getInstrument(libraryIndex: Int, instrumentIndex: Int): String

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("bssynth")
        }

        val INSTANCE = BssynthPlayer()
        fun  shared(): BssynthPlayer {
            return INSTANCE
        }
    }
}

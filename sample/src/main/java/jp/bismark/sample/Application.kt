//
// Copyright (c) 2020 bismark LLC All rights reserved
//

package jp.bismark.sample

import android.content.Context
import android.media.AudioManager
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import java.io.File
import java.io.IOException
import jp.bismark.bssynth.BssynthPlayer

class Application : android.app.Application() {

    val bssynth = BssynthPlayer.shared()

    override fun onCreate() {
        super.onCreate()

        val am = getSystemService(Context.AUDIO_SERVICE) as AudioManager
        var sampleRate = Integer.parseInt(am.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE))
        if (sampleRate == 0) sampleRate = 44100 // Use a default value if property not found
        var framesPerBuffer =
            Integer.parseInt(am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER))
        if (framesPerBuffer == 0) framesPerBuffer = 256 // Use default

        // Start Synthesizer
        var err = bssynth.initialize(this, this.applicationContext.assets, sampleRate, framesPerBuffer, "GeneralUser GS SoftSynth v1.44.sf2")
        if (err != 0) {
            Toast.makeText(this, "Synthesizer engine initialize error (#$err)", Toast.LENGTH_LONG)
                .show()
        } else {
            // Set default MIDI File
            err = bssynth.setFileFromAssets(this.applicationContext.assets, "sample.mid");
            if (err != 0) {
                Toast.makeText(this, "Set MIDI file error (#$err)", Toast.LENGTH_LONG).show()
            }
        }
    }

    override fun onTerminate() {
        bssynth.finalize()
        super.onTerminate()
    }

}
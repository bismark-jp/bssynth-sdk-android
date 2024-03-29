//
// Copyright (c) 2020 bismark LLC All rights reserved
//

package jp.bismark.sample

import android.content.pm.ActivityInfo
import android.os.Bundle
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import jp.bismark.bssynth.BssynthPlayer
import jp.bismark.bssynth.Instrument
import jp.bismark.sample.databinding.ActivityPartBinding

class PartActivity : AppCompatActivity() {

    private lateinit var binding: ActivityPartBinding

    var module = 0
    var part = 0
    val bssynth = BssynthPlayer.shared()
    val instrumentList: ArrayList<Instrument> = ArrayList()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityPartBinding.inflate(layoutInflater)
        setContentView(binding.root)

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT)

        module = intent.getIntExtra("module", 0)
        part = intent.getIntExtra("part", 0)

        this.title = "%02d".format(part + 1)

        binding.muteSwitch.isChecked = (bssynth.getMute(module, part) == 1)
        binding.muteSwitch.setOnCheckedChangeListener { _, isChecked ->
            when (isChecked) {
                true -> bssynth.setMute(module, part, 1)
                else -> bssynth.setMute(module, part, 0)
            }
        }

        binding.soloSwitch.isChecked = (bssynth.getSolo(module, part) == 1)
        binding.soloSwitch.setOnCheckedChangeListener { _, isChecked ->
            when (isChecked) {
                true -> bssynth.setSolo(module, part, 1)
                else -> bssynth.setSolo(module, part, 0)
            }
        }

        binding.volumeSeekBar.progress = bssynth.getControlChangeMessage(module, part, 7)
        binding.volumeSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            }
            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                val value = seekBar!!.progress
                bssynth.insertChannelMessage(module, 0xB0+part, 0x07, value)
            }
        })

        val instruments = bssynth.getLibraryNumInstruments(0)
        for (i in 0..instruments - 1) {
            val properties = bssynth.getInstrument(0, i)
            val propertiesList = properties.split(",")
            if (propertiesList.count() == 5) {
                val isDrum = propertiesList[0].toInt()
                val bankMsb = propertiesList[1].toInt()
                val bankLsb = propertiesList[2].toInt()
                val program = propertiesList[3].toInt()
                val name = propertiesList[4]
                val useForRhythmPart = bssynth.getUseForRhythmPart(module, part)
                if (useForRhythmPart == 0 && isDrum == 0) {
                    instrumentList.add(Instrument(bankMsb, bankLsb, program, name))
                } else if (useForRhythmPart != 0 && isDrum != 0) {
                    instrumentList.add(Instrument(bankMsb, bankLsb, program, name))
                }
            }
        }
        binding.listView.adapter = InstrumentAdapter(this, instrumentList)
        binding.listView.setOnItemClickListener { _, _, position, _ ->
            val instrument = instrumentList[position]
            bssynth.insertChannelMessage(module, 0xB0 + part, 0x00, instrument.bankMsb)
            bssynth.insertChannelMessage(module, 0xB0 + part, 0x20, instrument.bankLsb)
            bssynth.insertChannelMessage(module, 0xC0 + part, instrument.program, 0x00)
        }

        val bankMsb = bssynth.getControlChangeMessage(module, part, 0x00)
        val bankLsb = bssynth.getControlChangeMessage(module, part, 0x20)
        val program = bssynth.getProgramChangeMessage(module, part)
        instrumentList.forEachIndexed { index, it ->
            if (it.bankMsb == bankMsb && it.bankLsb == bankLsb && it.program == program) {
                binding.listView.setSelection(index)
                binding.listView.setItemChecked(index, true)
                return@forEachIndexed
            }
        }
    }
}

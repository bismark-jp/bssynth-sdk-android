//
// Copyright (c) 2020 bismark LLC All rights reserved
//

package jp.bismark.sample

import android.app.AlertDialog
import android.content.Intent
import android.content.pm.ActivityInfo
import android.content.pm.PackageManager
import android.Manifest.permission.READ_EXTERNAL_STORAGE
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.View
import android.widget.ArrayAdapter
import android.widget.SeekBar
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.nbsp.materialfilepicker.MaterialFilePicker
import com.nbsp.materialfilepicker.ui.FilePickerActivity
import kotlinx.android.synthetic.main.activity_main.*
import java.util.regex.Pattern
import jp.bismark.bssynth.BssynthPlayer

class MainActivity : AppCompatActivity() {

    val bssynth = BssynthPlayer.shared()

    var partItems: ArrayList<String> = ArrayList()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT)

        // GUI
        button_file.setOnClickListener(View.OnClickListener { v ->
            if (0 < bssynth.isPlaying()) {
                Toast.makeText(
                    v.context,
                    "Cannot select file, because player is being used",
                    Toast.LENGTH_LONG
                ).show()
                return@OnClickListener
            }

            checkPermissionsAndOpenFilePicker()
        })

        button_start.setOnClickListener(View.OnClickListener { v ->
            if (bssynth.start() != 0) {
                Toast.makeText(v.context, "Start error", Toast.LENGTH_LONG).show()
            }
        })

        button_stop.setOnClickListener(View.OnClickListener { v ->
            if (bssynth.stop() != 0) {
                Toast.makeText(v.context, "Stop error", Toast.LENGTH_LONG).show()
            }
        })

        seekBar1.setMax(bssynth.getTotalClocks())
        seekBar1.setProgress(0)
        seekBar1.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
            }
            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                val value = seekBar!!.progress
                bssynth.seek(value)
            }
        })

        button_key_down.setOnClickListener(View.OnClickListener { v ->
            var value = bssynth.getKeyControl()
            if (-5 < value) {
                value--
                bssynth.setKeyControl(value)
                Toast.makeText(v.context, "Key Control = $value", Toast.LENGTH_SHORT).show()
            }
        })

        button_key_up.setOnClickListener(View.OnClickListener { v ->
            var value = bssynth.getKeyControl()
            if (value < 5) {
                value++
                bssynth.setKeyControl(value)
                Toast.makeText(v.context, "Key Control = $value", Toast.LENGTH_SHORT).show()
            }
        })

        button_speed_down.setOnClickListener(View.OnClickListener { v ->
            var value = bssynth.getSpeedControl()
            if (-10 < value) {
                value -= 2
                bssynth.setSpeedControl(value)
                Toast.makeText(v.context, "Speed Control = $value", Toast.LENGTH_SHORT).show()
            }
        })

        button_speed_up.setOnClickListener(View.OnClickListener { v ->
            var value = bssynth.getSpeedControl()
            if (value < 10) {
                value += 2
                bssynth.setSpeedControl(value)
                Toast.makeText(v.context, "Speed Control = $value", Toast.LENGTH_SHORT).show()
            }
        })

        // timer
        val mainHandler = Handler(Looper.getMainLooper())
        Handler(Looper.getMainLooper()).post(object : Runnable {
            override fun run() {
                if (!seekBar1.isPressed()) seekBar1.setProgress(bssynth.getCurrentClocks())
                mainHandler.postDelayed(this, 1000)
            }
        })

        for (part in 1..16) partItems.add("%02d".format(part))
        listView.adapter = ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, partItems)
        listView.setOnItemClickListener { _, _, position, _ ->
            val intent = Intent(this, PartActivity::class.java).apply {
                putExtra("module", position / 16)
                putExtra("part", position % 16)
            }
            startActivity(intent)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        when (requestCode) {
            PERMISSIONS_REQUEST_CODE -> {
                if (grantResults.size > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    openFilePicker()
                } else {
                    showError()
                }
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        if (requestCode == FILE_PICKER_REQUEST_CODE && resultCode == RESULT_OK) {
            data ?: throw IllegalArgumentException("data must not be null")

            val path = data.getStringExtra(FilePickerActivity.RESULT_FILE_PATH)

            if (path != null) {
                Log.d("Path: ", path)
                if (bssynth.setFileFromPath(path) == 0) {
                    Toast.makeText(this, "Set MIDI file to " + path, Toast.LENGTH_LONG).show()

                    // Reset seek bar for the new midi file
                    seekBar1.setMax(bssynth.getTotalClocks())
                    seekBar1.setProgress(0)
                } else {
                    Toast.makeText(this, "Cannot Set MIDI file to " + path, Toast.LENGTH_LONG).show()
                }
            }
        }
    }

    private fun checkPermissionsAndOpenFilePicker() {
        val permission = READ_EXTERNAL_STORAGE
        if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
            if (ActivityCompat.shouldShowRequestPermissionRationale(this, permission)) {
                showError()
            } else {
                ActivityCompat.requestPermissions(this, arrayOf(permission), PERMISSIONS_REQUEST_CODE)
            }
        } else {
            openFilePicker()
        }
    }

    private fun showError() {
        Toast.makeText(this, "Allow external storage reading", Toast.LENGTH_SHORT).show()
    }

    private fun openFilePicker() {
        MaterialFilePicker()
            .withActivity(this)
            .withCloseMenu(true)
            .withFilter(Pattern.compile(".*\\.(mid|MID)$"))
            .withTitle("Select MIDI File")
            .withRequestCode(FILE_PICKER_REQUEST_CODE)
            .start()
    }

    companion object {
        private const val PERMISSIONS_REQUEST_CODE = 0
        private const val FILE_PICKER_REQUEST_CODE = 1
    }
}

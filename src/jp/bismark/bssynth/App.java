package jp.bismark.bssynth;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.app.Application;
import android.content.res.AssetManager;
import android.widget.Toast;

import org.puredata.android.io.AudioParameters;

public class App extends Application {
	
	private int part = 0;
	
    @Override
    public void onCreate () {
    	bssynthInit ();
    	bssynthStart ();
    }

    @Override
    public void onTerminate () {
    	bssynthStop ();
    	bssynthExit ();
    }
	
    public void bssynthInit () {
    	// Use org.puredata.android.io.AudioParameters for low-latency
    	int sampleRate = AudioParameters.suggestSampleRate ();
    	int blockSize = AudioParameters.suggestOutputBufferSize (sampleRate);

		// Load SoundLibrary
    	String foldername = "GeneralUser GS 1.44 SoftSynth/";
    	String filename = "GeneralUser GS SoftSynth v1.44.sf2";
    	String libraryPath = this.getFilesDir () + "/" + filename;
        File file = new File (libraryPath);
        if (file.exists() == false) {
            try {
            	copy2Local (foldername + filename ,filename);
            }
            catch (IOException e) {
                Toast.makeText (this, "Failed to copy sound library", Toast.LENGTH_LONG).show ();
            }
        }

        // Start Synthesizer
        if (Initialize (libraryPath, sampleRate, blockSize) != 0) {
            Toast.makeText (this, "Synthesizer engine initalize error", Toast.LENGTH_LONG).show ();
        }
        else {
            Toast.makeText (this, "Synthesizer engine initalized", Toast.LENGTH_LONG).show ();
        }
    }

    public void bssynthExit () {
    	Finalize ();
    }

    public void bssynthStart () {
    	Start ();

    	SetReverb (1); // reverb on
    	SetChorus (1); // chorus on

    	bssynthSetChannelMessage (0xB0, 0x00, 0x00); // bank select msb = 0
    	bssynthSetChannelMessage (0xB0, 0x20, 0x00); // bank select lsb = 0
    	bssynthSetChannelMessage (0xC0, 0x00, 0x00); // program = 0
		
    	bssynthSetChannelMessage (0xB0, 0x07, 0x64); // volume = 100
    	bssynthSetChannelMessage (0xB0, 0x0a, 0x40); // panpot = center
    	bssynthSetChannelMessage (0xB0, 0x5b, 0x28); // reverb send = 40
    	bssynthSetChannelMessage (0xB0, 0x5d, 0x00); // chorus send = 0
    }

    public void bssynthStop () {
    	Stop ();
    }

    public void bssynthSetChannelMessage (int status, int data1, int data2) {
    	SetChannelMessage (status + part, data1, data2);
    }
    
	private void copy2Local (String src, String dst) throws IOException { 
    	AssetManager as = getResources ().getAssets ();
    	InputStream input = as.open (src);
    	FileOutputStream output = openFileOutput (dst, MODE_PRIVATE); 
    	int DEFAULT_BUFFER_SIZE = 1024 * 4; 
        
    	byte[] buffer = new byte[DEFAULT_BUFFER_SIZE]; 
    	int n = 0; 
    	while (-1 != (n = input.read(buffer))) { 
    		output.write(buffer, 0, n); 
    	} 
    	output.close (); 
    	input.close ();
    }
    
    /** Native methods, implemented in jni folder */
    public native int Initialize (String libraryPath, int sampleRate, int blockSize);
    public native void Finalize ();

    public native int Start ();
    public native int Stop ();
    public native int IsPlaying ();

    public native void SetChannelMessage (int status, int data1, int data2);

    public native void SetReverb (int value);
    public native void SetChorus (int value);

    // bssynth library should be loaded from a class extended from "android.content.Context"
    static {
    	System.loadLibrary ("bssynth-jni");
    }	
}

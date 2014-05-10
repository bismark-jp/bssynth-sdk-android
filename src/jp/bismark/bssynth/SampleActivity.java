package jp.bismark.bssynth;

import android.app.Activity;
import android.os.Bundle;
import android.content.pm.ActivityInfo;

public class SampleActivity extends Activity implements KeyboardInterface {

    public void onCreate (Bundle savedInstanceState) {
        super.onCreate (savedInstanceState);
		setContentView (R.layout.sample);

        setRequestedOrientation (ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		KeyboardView keyboardView = (KeyboardView) findViewById (R.id.keyboardView);
        keyboardView.delegate = this;
    }

	public void note (int tag, int note, int velocity) {
        App app = (App) this.getApplication ();
        app.SetChannelMessage (0x90, note, velocity);
	}
}

package jp.bismark.bssynth;

import android.content.Context;
import android.graphics.*;
import android.view.*;
import android.util.*;
import android.graphics.Paint.FontMetrics;

public class KeyboardView extends SurfaceView implements SurfaceHolder.Callback, Runnable {

	public KeyboardInterface delegate = null;
	public int tag = 0;
	
	private Thread mainLoop = null;
	private SurfaceHolder holder;
	protected Boolean needToDisplay = false;

	protected int width = 0;
	protected int height = 0;
	protected int offset = 0;
	protected int keyboardWidth = 0;
	
	private final static int numberOfTouches = 20;
	private int previousScans[];

	public float range = 15.f / 7.f; // [octave]
	public float base = 28.f; // [white-key: 0 ~ 75]

	private int lowWhiteKey;
	private int numberOfWhiteKeys;
	private int lowKey; // [key]
	private int highKey; // [key]
	
	private float whiteKeyWidth;
	private float whiteKeyHeight;
	private float blackKeyWidth;
	private float blackKeyHeight;
	private Bitmap whiteKeyImage = null;
	private Bitmap whitePressedKeyImage = null;
	private Bitmap blackKeyImage = null;
	private Bitmap blackPressedKeyImage = null;

	private Paint textPaint;
	private FontMetrics fontMetrics;
	private float labelBaseY;
	private float labelImageBaseY;

	private boolean scaleOld[];
	private boolean scaleNew[];
	
	public boolean touchesBetweenBlackKeys = true;
	public boolean showsLabel = true;
	public boolean showsLabelOnlyC = true;

	public KeyboardView (Context context) {
		this (context, null);
	}
 
	public KeyboardView (Context context, AttributeSet attrs) {
		this (context, attrs, 0);
	}
 
	public KeyboardView (Context context, AttributeSet attrs, int defStyle) {
		super (context, attrs, defStyle);

		holder = getHolder();
		holder.addCallback(this);
		setFocusable(true);
		requestFocus();

		scaleOld = new boolean[128];
		scaleNew = new boolean[128];
		
		previousScans = new int[numberOfTouches];
		for (int i = 0; i < numberOfTouches; i++) {
			previousScans[i] = -1;
		}

		textPaint = new Paint (Paint.ANTI_ALIAS_FLAG);
		textPaint.setTextSize (17);
		textPaint.setColor (Color.BLACK);
		Typeface typeface = Typeface.create (Typeface.DEFAULT, Typeface.NORMAL);
		textPaint.setTypeface (typeface);
		fontMetrics = textPaint.getFontMetrics ();		
	}
 
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int _width, int _height) {
		width = _width;
		height = _height;
		setBase (base);
		setRange (range);
		needToDisplay = true;
	}
 
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		width = getWidth ();
		height = getHeight ();
		setBase (base);
		setRange (range);
		needToDisplay = true;
	}
 
	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {}
 
	@Override
	public void run() {
		while (true) {
			if (needToDisplay) {

				Canvas canvas = holder.lockCanvas();
				if (canvas != null) {
					needToDisplay = false;
					updateDisplay (canvas);
	    	    	holder.unlockCanvasAndPost (canvas);
				}
    		}

            try {
                Thread.sleep (10);
            }
            catch (InterruptedException e) {
            }
    	}
    }    

	protected void updateDisplay (Canvas canvas) {
		{
			float x = (lowWhiteKey - base) * whiteKeyWidth + offset;
			float y = 0;
			for (int i = 0; i <= numberOfWhiteKeys; i++) {
				int note = whiteScale2Scale (lowWhiteKey + i);
				if (scaleOld[note]) {
					// draw whilteKey (pressed)
					canvas.drawBitmap (whitePressedKeyImage, x, y, null);
				}
				else {
					// draw whilteKey (not pressed)
					canvas.drawBitmap (whiteKeyImage, x, y, null);
				}
				
				if (showsLabel) {
					int note0 = note % 12;
					if (showsLabelOnlyC == false || (showsLabelOnlyC && note0 == 0)) {
						String label;
						switch (note % 12) {
						case 0: label = "C"; break;
						case 1: label = "C#"; break;
						case 2: label = "D"; break;
						case 3: label = "D#"; break;
						case 4: label = "E"; break;
						case 5: label = "F"; break;
						case 6: label = "F#"; break;
						case 7: label = "G"; break;
						case 8: label = "G#"; break;
						case 9: label = "A"; break;
						case 10: label = "A#"; break;
						case 11: label = "B"; break;
						default: label = ""; break;
						}
						label += note / 12 - 1;
						canvas.drawText (label, x + (whiteKeyWidth - textPaint.measureText (label)) / 2, y + labelBaseY, textPaint);
					}
				}
				x += whiteKeyWidth;
			}
		}

		{
			float y = (float) 0;
			for (int i = 0; i <= numberOfWhiteKeys; i++) {
				float x = -1.f;
				int key = whiteScale2Scale (lowWhiteKey + i);
				if (127 <= key) continue;

				switch (key % 12) {
				case 0: // C
					x = (i + 1) * whiteKeyWidth - blackKeyWidth * 2.f / 3.f;
					break;
    			case 2: // D
    				x = (i + 1) * whiteKeyWidth - blackKeyWidth / 3.f;
    				break;
    			case 4: // E
    				break;
    			case 5: // F
    				x = (i + 1) * whiteKeyWidth - blackKeyWidth * 2.f / 3.f;
    				break;
    			case 7: // G
    				x = (i + 1) * whiteKeyWidth - blackKeyWidth / 2.f;
    				break;
    			case 9: // A
    				x = (i + 1) * whiteKeyWidth - blackKeyWidth / 3.f;
    				break;
    			case 11: // B
    				break;
    			}

    			if (0.f < x) {
    				x += offset;
    				if (scaleOld[key + 1]) {
    			    	// draw blackKey (pressed)
        				canvas.drawBitmap (blackPressedKeyImage, x + (lowWhiteKey - base) * whiteKeyWidth, y, null);
    				}
    				else {
    			    	// draw blackKey (not pressed)
        				canvas.drawBitmap (blackKeyImage, x + (lowWhiteKey - base) * whiteKeyWidth, y, null);
    				}
    			}
    		}
    	}
	}
	
	@Override
	public boolean onTouchEvent (MotionEvent event) {
		int count = event.getPointerCount();
		int action = event.getAction();
		int id = (action & MotionEvent.ACTION_POINTER_INDEX_MASK ) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
		
		action &= MotionEvent.ACTION_MASK;
/*
		switch(action) {
		case MotionEvent.ACTION_DOWN:
			Log.v("KeyboardView", "Touch Down" + " count=" + count + ", id=" + id);
			break;
		case MotionEvent.ACTION_POINTER_DOWN:
			Log.v("KeyboardView", "Touch PTR Down" + " count=" + count + ", id=" + id);
			break;
		case MotionEvent.ACTION_UP:
			Log.v("KeyboardView", "Touch Up" + " count=" + count + ", id=" + id);
			break;
		case MotionEvent.ACTION_POINTER_UP:
			Log.v("KeyboardView", "Touch PTR Up" + " count=" + count + ", id=" + id);
			break;
		case MotionEvent.ACTION_MOVE:
			Log.v("KeyboardView", "Touch Move" + " count=" + count + ", id=" + id);
			break;
		}
*/
		for(int i = 0; i < count; i++) {
			int _id = event.getPointerId (i);
			if (id == i) {
//				Log.v("KeyboardView", "   X=" + event.getX(i) + ", Y=" + event.getY(i) + ", id=" + event.getPointerId(i) );
				doScan (event, _id, action, event.getX (i), event.getY (i));
			}
			else {
//				Log.v("KeyboardView", "   X=" + event.getX(i) + ", Y=" + event.getY(i) + ", id=" + event.getPointerId(i) + " MOVE");
				doScan (event, _id, MotionEvent.ACTION_MOVE, event.getX (i), event.getY (i));
			}
		}
		
		{
	    	int s;
	    	for (s = lowKey; s <= highKey; s++) 
	    	{
	    		if (!scaleOld[s] && scaleNew[s]) {
	    			// note on
	    			delegate.note (tag, s, 127);
	        		needToDisplay = true;
	        		scaleOld[s] = scaleNew[s];
	    		}
	    		else if (scaleOld[s] && !scaleNew[s]) {
	    			// note off
	    			delegate.note (tag, s, 0);
	        		needToDisplay = true;
	        		scaleOld[s] = scaleNew[s];
	    		}
	    	}
		}
		return true;
	}
	
    @Override
	protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
		super.onLayout(changed, left, top, right, bottom);
		
		if (width != getWidth () || height != getHeight ()) {
			width = getWidth ();
			height = getHeight ();
//			Log.v("KeyboardView", "onLayout: width = " + width + ", height =" + height);			
			setBase (base);
			setRange (range);
		}
	}

    public int whiteScale2Scale (int whiteScale)
    {
    	int offset2Scale[] = {0, 2, 4, 5, 7, 9, 11, 12};
    	int octave = whiteScale / 7;
    	int offset = whiteScale % 7;
    	int scale = octave * 12 + offset2Scale[offset];
    	return scale;
    } 

    public void setBase (float value, Boolean invalidate) {
    	setBase (value);
    	needToDisplay = invalidate;
    }

    private void setBase (float value) {
    	for (int s = 0; s < 128; s++) {
    		if (scaleOld[s]) {
    			delegate.note (tag, s, 0);
    			scaleOld[s] = scaleNew[s] = false;
    		}
    	}
    	
    	if (value < 0.f) value = 0.f;
    	else if (value + (range * 7.f) + 1 >= 76.f) {
    		value = 75.f - (range * 7.f);
    	}

    	base = value;

    	lowKey = whiteScale2Scale ((int) base);
    	highKey = whiteScale2Scale ((int) (base + range * 7.f - 0.5f));
//		Log.v("KeyboardView", "setBase: lowKey = " + lowKey + ", highKey =" + highKey);			

    	lowWhiteKey = (int) base;
    }

    public void setRange (float value, Boolean invalidate) {
    	setRange (value);
    	needToDisplay = invalidate;
    }
    
	protected void setRange (float value)
	{
		if (keyboardWidth == 0) keyboardWidth = width;
		
		for (int s = 0; s < 128; s++) {
    		if (scaleOld[s]) {
    			delegate.note (tag, s, 0);
    			scaleOld[s] = scaleNew[s] = false;
    		}
    	}

    	if (value < 0.5f) value = 0.5f;
    	else if ((22.f / 7.f) < value) value = 22.f / 7.f;

    	if (base + (range * 7.f) + 1 >= 76.f) {
    		base = 75.f - (range * 7.f);
    	}

    	range = value;

    	highKey = whiteScale2Scale ((int) (base + range * 7.f - 0.5f));

    	numberOfWhiteKeys = (int) (range * 7.f) + 1;
    		
    	whiteKeyWidth = keyboardWidth / (range * 7.f);
    	whiteKeyHeight = height;
    	blackKeyWidth = whiteKeyWidth * 2.f / 3.f;
    	blackKeyHeight = whiteKeyHeight * 0.6f;

    	{
    		Bitmap bitmap = BitmapFactory.decodeResource (getResources (), R.drawable.keyboard_view_white); 
    		whiteKeyImage = Bitmap.createScaledBitmap (bitmap, (int) whiteKeyWidth, (int) whiteKeyHeight, true);
    	}

    	{
    		Bitmap bitmap = BitmapFactory.decodeResource (getResources (), R.drawable.keyboard_view_white_pressed); 
    		whitePressedKeyImage = Bitmap.createScaledBitmap (bitmap, (int) whiteKeyWidth, (int) whiteKeyHeight, true);
    	}

    	{
    		Bitmap bitmap = BitmapFactory.decodeResource (getResources (), R.drawable.keyboard_view_black); 
    		blackKeyImage = Bitmap.createScaledBitmap (bitmap, (int) blackKeyWidth, (int) blackKeyHeight, true);
    	}

    	{
    		Bitmap bitmap = BitmapFactory.decodeResource (getResources (), R.drawable.keyboard_view_black_pressed); 
    		blackPressedKeyImage = Bitmap.createScaledBitmap (bitmap, (int) blackKeyWidth, (int) blackKeyHeight, true);
    	}
    	
    	{
    		float labelImageWidth = whiteKeyWidth * 2 / 3;
    		float labelImageHeight = labelImageWidth * 2 / 3;

    		labelImageBaseY = whiteKeyHeight * 5 / 6;
    		
			labelBaseY = labelImageBaseY + labelImageHeight / 2 - (fontMetrics.ascent + fontMetrics.descent) / 2;
    	}

    	if (mainLoop == null) {
    		mainLoop = new Thread(this);
    		mainLoop.start();
    	}
	}
    
    public void setShowsLabel (Boolean value)
    {
    	showsLabel = value;
    	needToDisplay = true;
    }

    public void setShowsLabelOnlyC (Boolean value)
    {
    	showsLabelOnlyC = value;
    	needToDisplay = true;
    }

    public void setTouchesBetweenBlackKeys (Boolean value)
    {
    	touchesBetweenBlackKeys = value;
    }
    
    private int scan (float X, float Y)
    {
    	int x = (int) (X + 0.5f);
    	int y = (int) (Y + 0.5f);
    	
    	int whitescale;
    	int blackscale = 0;
    	boolean overranged = false;
    	int octave = 0;

    	if (x < 0) {
    		x = 0;
    		overranged = true;
    	}
    	else if (keyboardWidth < x) {
    		x = keyboardWidth;
    		overranged = true;
    	}

    	if (y < 0) {
    		y = 0;
    		overranged = true;
    	}
    	else if (height < y) {
    		y = height;
    		overranged = true;
    	}
    	
    	x -= (lowWhiteKey - base) * whiteKeyWidth;
    	whitescale = (int) ((float) x / whiteKeyWidth);

    	if (blackKeyHeight > y) {
    		int offset = x - (int) (whitescale * whiteKeyWidth);
    		if (!touchesBetweenBlackKeys) { // blackPrior
    			switch (whiteScale2Scale ((int) (base + whitescale)) % 12) {
    			case 0: // C
    			case 5: // F
    				blackscale = 1;
    				break;
    			case 4: // E
    			case 11: // B
    				blackscale = -1;
    				break;
    			case 12: // C
    				overranged = true;
    				break;
    			default:
    				if (offset < (whiteKeyWidth * 0.5f)) {
    					blackscale = -1;
    				}
    				else {
    					blackscale = 1;
    				}
    				break;
    			}
    		}
    		else {
    			switch (whiteScale2Scale ((int) (base + whitescale)) % 12) {
    			case 0: // C
    			case 5: // F
    				if ((whiteKeyWidth - 2 * blackKeyWidth / 3) < offset) {
    					blackscale = 1;
    				}
    				break;
    			case 2: // D
    				if (offset < (blackKeyWidth / 3)) {
    					blackscale = -1;
    				}
    				else if ((whiteKeyWidth - blackKeyWidth / 3) < offset) {
    					blackscale = 1;
    				}
    				break;
    			case 4: // E
    				if (offset < (2 * blackKeyWidth / 3)) {
    					blackscale = -1;
    				}
    				break;
    			case 7: // G
    				if (offset < (blackKeyWidth / 3)) {
    					blackscale = -1;
    				}
    				else if ((whiteKeyWidth - blackKeyWidth / 2) < offset) {
    					blackscale = 1;
    				}
    				break;
    			case 9: // A
    				if (offset < (blackKeyWidth / 2)) {
    					blackscale = -1;
    				}
    				else if ((whiteKeyWidth - blackKeyWidth / 3) < offset) {
    					blackscale = 1;
    				}
    				break;
    			case 11: // B
    				if (offset < (2 * blackKeyWidth / 3)) {
    					blackscale = -1;
    				}
    				break;
    			}
    		}
    	}

    	if (overranged) return -1;
    	else return whiteScale2Scale ((int) (base + whitescale)) + blackscale + octave;
    }

	private void doScan (MotionEvent event, int id, int action, float x, float y)
	{
    	int keyboardScan = scan (x - offset, y);
    	int previousKeyboardScan;

    	switch (action) {
    	case MotionEvent.ACTION_DOWN: /* began */
    	case MotionEvent.ACTION_POINTER_DOWN: /* began */
    		if (0 <= keyboardScan) {
    			scaleNew[keyboardScan] = true;
//    			Log.v("KeyboardView", "      DOWN" + keyboardScan + "ON");
    		}
    		break;
    	case MotionEvent.ACTION_MOVE: /* move */
        	previousKeyboardScan = previousScans[id];
    		if (0 <= previousKeyboardScan) {
    			scaleNew[previousKeyboardScan] = false;
//    			Log.v("KeyboardView", "      MOVE" + previousKeyboardScan + "OFF");
    		}
    		if (0 <= keyboardScan) {
    			scaleNew[keyboardScan] = true;
//    			Log.v("KeyboardView", "      MOVE" + keyboardScan + "ON");
    		}
    		break;
    	case MotionEvent.ACTION_UP: /* end */
    	case MotionEvent.ACTION_POINTER_UP:
        	previousKeyboardScan = previousScans[id];
    		if (0 <= previousKeyboardScan) {
    			scaleNew[previousKeyboardScan] = false;
//    			Log.v("KeyboardView", "      UP" + previousKeyboardScan + "OFF");
    		}
    		if (0 <= keyboardScan) {
    			scaleNew[keyboardScan] = false;
//    			Log.v("KeyboardView", "      UP" + keyboardScan + "OFF");
    		}
    		break;
    	}
    	
    	previousScans[id] = keyboardScan;
    }
}

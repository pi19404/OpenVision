/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.openvision.androidgesture;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.MotionEvent;
import android.gesture.GestureOverlayView;
import android.gesture.Gesture;
import android.gesture.GestureLibrary;
import android.gesture.GestureStroke;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/** 
 * 
 * @author pi19404
 *
 */
public class CreateGestureActivity extends Activity {
    private static final float LENGTH_THRESHOLD = 120.0f;

    public static final String DIR=Environment.getExternalStorageDirectory().getPath()+"/AndroidGesture/v1";
    private Gesture mGesture;
    private View mDoneButton;
    private EditText eText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.store_gesture);

        mDoneButton = findViewById(R.id.done);
        eText = (EditText) findViewById(R.id.gesture_name);
        GestureOverlayView overlay = (GestureOverlayView) findViewById(R.id.gestures);
        overlay.addOnGestureListener(new GesturesProcessor());
        GestureLibraryInterface.setDirectory(DIR);
    }
    


    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        
        if (mGesture != null) {
            outState.putParcelable("gesture", mGesture);
        }
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        
        mGesture = savedInstanceState.getParcelable("gesture");
        if (mGesture != null) {
            final GestureOverlayView overlay =
                    (GestureOverlayView) findViewById(R.id.gestures);
            overlay.post(new Runnable() {
                public void run() {
                    overlay.setGesture(mGesture);
                }
            });

            mDoneButton.setEnabled(true);
        }
    }

   
    @SuppressWarnings({"UnusedDeclaration"})
    public void addGesture(View v) {
    	Log.e("CreateGestureActivity","Adding Gestures");
    	extractGestureInfo();


    }
    
    @SuppressWarnings({"UnusedDeclaration"})
    public void cancelGesture(View v) {
        setResult(RESULT_CANCELED);
        finish();
    }
    

    private class GesturesProcessor implements GestureOverlayView.OnGestureListener {
        public void onGestureStarted(GestureOverlayView overlay, MotionEvent event) {
            mDoneButton.setEnabled(false);
            mGesture = null;
        }

        public void onGesture(GestureOverlayView overlay, MotionEvent event) {
        }

        public void onGestureEnded(GestureOverlayView overlay, MotionEvent event) {
            mGesture = overlay.getGesture();
            if (mGesture.getLength() < LENGTH_THRESHOLD) {
                overlay.clear(false);
            }
            mDoneButton.setEnabled(true);
        }

        public void onGestureCancelled(GestureOverlayView overlay, MotionEvent event) {
        }
    }
    


    void extractGestureInfo()
    {
    	
    	String str = eText.getText().toString();		
    	ArrayList<GestureStroke> list=mGesture.getStrokes();
    	ArrayList<Float> location= new ArrayList<Float>();
    	ArrayList<Long> time= new ArrayList<Long>();;
    	
    	for(int i=0;i<list.size();i++)
    	{
    		GestureStroke stroke=list.get(i);
    		
    		Log.e("AA","stroke length "+stroke.length+":::"+stroke.points.length);
    		//Path p=stroke.getPath();
    		//float[] points=new float[(int)(2*stroke.length)];
    		long[] timestamps=new long[(int)stroke.points.length/2];
    		
    		for(int k=0;k<stroke.points.length/2;k++)
    		{
    			
    			location.add(stroke.points[2*k]);
    			location.add(stroke.points[2*k+1]);
    			time.add(timestamps[k]);
    		}
    		
    	}
    	Log.e("extractGestureInfo","before addGesture");
    	GestureLibraryInterface.addGesture(location,time,str);
    	Log.e("extractGestureInfo","after addGesture");
    }
    
}

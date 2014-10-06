package com.openvision.androidgesture;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;


import android.app.Activity;
import android.app.ListActivity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.gesture.Gesture;
import android.gesture.GestureLibraries;
import android.gesture.GestureLibrary;
import android.gesture.GestureOverlayView;
import android.gesture.GestureOverlayView.OnGesturePerformedListener;
import android.gesture.Prediction;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.media.ThumbnailUtils;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import android.widget.Toast;




public class OpenVisionGestureA extends ListActivity   {
    private static final int STATUS_SUCCESS = 0;
    private static final int STATUS_CANCELLED = 1;
    private static final int STATUS_NO_STORAGE = 2;
    private static final int STATUS_NOT_LOADED = 3;	
    private static final int REQUEST_NEW_GESTURE = 1;
    
    private TextView mEmpty;
    
	
	
	private GesturesAdapter mAdapter;
	private GesturesLoadTask mTask;
	
	public class ImageFileFilter implements FileFilter
	{
	private final String[] okFileExtensions = 
	new String[] {"jpg", "png", "gif","bmp"};
	
	public boolean accept(File file)
	{
	for (String extension : okFileExtensions)
	{
		if(file.getName().toLowerCase().endsWith(extension))
		{
		return true;
		}
	}
		return false;
	}
	}
	
	public class DirFilter implements FileFilter
	{
	private final String[] okFileExtensions = 
	new String[] {"jpg", "png", "gif","bmp"};
	
	public boolean accept(File file)
	{
		if(file.isDirectory() && file.getName()!="." && file.getName()!="..")
			return true;
		else 
			return false;
	}
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_vision_gesture);
        mAdapter = new GesturesAdapter(this);
        setListAdapter(mAdapter);
        //mLibrary = GestureLibraries.fromRawResource(this, R.raw.gestures);
        //if (!mLibrary.load()) {
        ////  finish();
       // }
      
        //GestureOverlayView gestures = (GestureOverlayView) findViewById(R.id.gestures);
        //gestures.addOnGesturePerformedListener(this);
        
        displayGestures();
        registerForContextMenu(getListView());
        findViewById(R.id.addButton).setEnabled(true);
   
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (mTask != null && mTask.getStatus() != GesturesLoadTask.Status.FINISHED) {
            mTask.cancel(true);
            mTask = null;
        }
        
    }
    
    //This function display all the stored gestures
    public void  displayGestures() {
    	

        if (mTask != null && mTask.getStatus() != GesturesLoadTask.Status.FINISHED) {
            mTask.cancel(true);
        }        
        mTask = (GesturesLoadTask) new GesturesLoadTask().execute();

    	

       
    }
    
    static class NamedGesture {
        String name;
        Long id;
       // Gesture gesture;
    }

    
    private class GesturesLoadTask extends AsyncTask<Void, NamedGesture, Integer> {
        private int mThumbnailSize;
        private int mThumbnailInset;
        private int mPathColor;

        @Override
        protected void onPreExecute() {
            super.onPreExecute();

            final Resources resources = getResources();
            mPathColor = resources.getColor(R.color.gesture_color);
            mThumbnailInset = (int) resources.getDimension(R.dimen.gesture_thumbnail_inset);
            mThumbnailSize = (int) resources.getDimension(R.dimen.gesture_thumbnail_size);

            findViewById(R.id.addButton).setEnabled(false);
            
            
            mAdapter.setNotifyOnChange(false);            
            mAdapter.clear();
        }

        @Override
        protected Integer doInBackground(Void... params) {
            if (isCancelled()) return STATUS_CANCELLED;
            if (!Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
                return STATUS_NO_STORAGE;
            }

            

            Long id=new Long(0);
        	File list = new File(CreateGestureActivity.DIR);
        	
        	
        	File[] files = list.listFiles(new DirFilter());
        	
        	for(int i=0;i<files.length;i++)
        	{
        		File[] list1=files[i].listFiles(new ImageFileFilter());
        		for(int k=0;k<list1.length;k++)
        		{
        		BitmapFactory.Options options = new BitmapFactory.Options();
        		options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        		Bitmap bitmap = BitmapFactory.decodeFile(list1[k].getPath(), options);
        		//f
        		
        		Bitmap ThumbImage = ThumbnailUtils.extractThumbnail(bitmap, mThumbnailSize, mThumbnailSize);
        		
        		
        		 final NamedGesture namedGesture = new NamedGesture();
                 //SnamedGesture.gesture = gesture;
                 namedGesture.id=id;
                 namedGesture.name = files[i].getName()+"_"+list1[k].getName();
                 Log.e("GG","adding gesture "+namedGesture.name);
                 mAdapter.addBitmap((Long)id, ThumbImage);
                 id=id+1;
                 publishProgress(namedGesture);
                 bitmap.recycle();
        		}        		
        	}

            return STATUS_SUCCESS;
        }

        
        @Override
        protected void onProgressUpdate(NamedGesture... values) {
            super.onProgressUpdate(values);

            final GesturesAdapter adapter = mAdapter;
            adapter.setNotifyOnChange(false);

            for (NamedGesture gesture : values) {
                adapter.add(gesture);
            }

            //adapter.sort(mSorter);
            adapter.notifyDataSetChanged();
        }
        private void checkForEmpty() {
            if (mAdapter.getCount() == 0) {
                mEmpty.setText(R.string.gestures_empty);
            }
        }

        @Override
        protected void onPostExecute(Integer result) {
            super.onPostExecute(result);

            if (result == STATUS_NO_STORAGE) {
                getListView().setVisibility(View.GONE);
                mEmpty.setVisibility(View.VISIBLE);
                mEmpty.setText(getString(R.string.gestures_error_loading,CreateGestureActivity.DIR));
            } else {
                findViewById(R.id.addButton).setEnabled(true);
                
                checkForEmpty();
            }
        }
    }
    
    private class GesturesAdapter extends ArrayAdapter<NamedGesture> {
        private final LayoutInflater mInflater;
        private final Map<Long, Drawable> mThumbnails = Collections.synchronizedMap(
                new HashMap<Long, Drawable>());

        public GesturesAdapter(Context context) {
            super(context, 0);
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        @SuppressWarnings("deprecation")
		void addBitmap(Long string, Bitmap bitmap) {
            mThumbnails.put(string, new BitmapDrawable(bitmap));
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.gestures_item, parent, false);
            }

            final NamedGesture gesture = getItem(position);
            final TextView label = (TextView) convertView;

            label.setTag(gesture);
            label.setText(gesture.name);
            label.setCompoundDrawablesWithIntrinsicBounds(mThumbnails.get(gesture.id),null, null, null);

            return convertView;
        }
    
    
    } 
    @SuppressWarnings({"UnusedDeclaration"})
    public void addGesture(View v) {
        Intent intent = new Intent(this, CreateGestureActivity.class);
        startActivityForResult(intent, REQUEST_NEW_GESTURE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        
        if (resultCode == RESULT_OK) {
            switch (requestCode) {
                case REQUEST_NEW_GESTURE:
                	displayGestures();
                    break;
            }
        }
    }
   

}

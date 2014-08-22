package com.openvision.androidgesture;

import java.util.ArrayList;



class Loader {
    private static boolean done = false;

    protected static synchronized void load() {
        if (done)
            return;
 
           	System.loadLibrary("opencv_core");
           	System.loadLibrary("opencv_imgproc");
           	System.loadLibrary("opencv_flann");
           	System.loadLibrary("opencv_highgui");
           	System.loadLibrary("opencv_features2d");
           	System.loadLibrary("opencv_ml");
           	System.loadLibrary("opencv_video");
           	System.loadLibrary("opencv_objdetect");
           	System.loadLibrary("opencv_calib3d");
           	System.loadLibrary("opencv_contrib");
           	System.loadLibrary("opencv_legacy");
           	
           	 System.loadLibrary("OpenVision");
           	
           
        done = true;
    }
}

public class GestureLibraryInterface {
    static{Loader.load();}
    public native static void addGesture(ArrayList<Float> location,ArrayList<Long> time,String name);

    public native static void setDirectory(String name);
    
    public native static void loadGesture();
    
    }
 


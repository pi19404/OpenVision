#include "GestureLibraryInterface.hpp"
UniStrokeGestureRecognizer recognizer;


/**
 * @brief method to return a float value from jobject
 * @param env JNI Environment points
 * @param jobject input argument to the function
 *
 * @return out is variable of type float
 */
float getFloat(JNIEnv *env,jobject value)
{

	 jclass cls=env->FindClass("java/lang/Float");

	 jmethodID mid = env->GetMethodID(cls, "floatValue", "()F");
	 if (mid == 0)
	    return -1;


	 float FloatValue = (float)env->CallFloatMethod(value, mid);

	 return FloatValue;
}

/**
 * @param env JNI Environment points
 * @param jobject input argument to the function
 *
 * @return out is variable of type long
 */

long getLong(JNIEnv *env,jobject value)
{
	 //jclass cls=env->GetObjectClass(value);
	jclass cls=env->FindClass("java/lang/Long");
	 jmethodID mid = env->GetMethodID(cls, "longValue", "()J");
	 if (mid == 0)
	    return -1;

	 long LongValue = (float)env->CallLongMethod(value, mid);
	 return LongValue;
}

/**
 *  @brief native interface function for the setUSerList function from java files.
 */
JNIEXPORT void JNICALL Java_com_openvision_androidgesture_GestureLibraryInterface_addGesture(JNIEnv * env, jobject jthiz, jobject locations,jobject time,jstring name) {
    vector<GesturePoint> points;
    LOGE("Logging addGesture native");
	char * template_name;
    template_name = (char *)env->GetStringUTFChars(name,NULL );

	int i;
	// Class ArrayList
	jclass cls_arraylist = env-> GetObjectClass(locations);
	jclass cls_arraylist1 = env-> GetObjectClass(time);
	// Method in class ArrayList
	jmethodID arraylist_get = env->GetMethodID(cls_arraylist, "get","(I)Ljava/lang/Object;");
	jmethodID arraylist_get1 = env->GetMethodID(cls_arraylist1, "get","(I)Ljava/lang/Object;");
	jmethodID arraylist_size = env->GetMethodID(cls_arraylist1, "size","()I");
	jint len = env->CallIntMethod(time,arraylist_size);


	//printf("get java ArrayList <User> object by C + +, then print it ..... / n");
	for (i = 0; i <len; i++) {

    jobject obj_usera = env->CallObjectMethod(locations,arraylist_get,2*i);
    jobject obj_userb = env->CallObjectMethod(locations,arraylist_get,2*i+1);
    jobject obj_user1 = env->CallObjectMethod(time,arraylist_get1,i);

    float x=getFloat(env,obj_usera);

    float y=getFloat(env,obj_userb);

    float time = getLong(env,obj_user1);

    GesturePoint p;
    p.position.x=x;
    p.position.y=y;
    p.time=time;
    points.push_back(p);
	}
	 LOGE("completed reading the array");
	recognizer.save(template_name,points);
	LOGE("completed saving the array");
}

/**
 *  setting the gesture template directory name
 */
JNIEXPORT void JNICALL Java_com_openvision_androidgesture_GestureLibraryInterface_setDirectory(JNIEnv *env,jobject jthiz,jstring name)
{
	char * path;
    path = (char *)env->GetStringUTFChars(name,NULL );
    recognizer.setPath(path);
}

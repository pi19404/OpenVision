/*
 * ImgUtils.hpp
 *
 *  Created on: 20-Aug-2014
 *      Author: pi19404
 */
#ifndef IMGUTILS_HPP_
#define IMGUTILS_HPP_
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include "dirent.h"

#include <iostream>
#include<string>
#include <vector>
#include "ImgUtils/logger.h"
//debug info for android systems define #define ANDROID=1;
//#define ANDROID=0
#ifdef ANDROID
#include <android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "JNI_DEBUGGING", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,   "JNI_DEBUGGING", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,    "JNI_DEBUGGING", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,    "JNI_DEBUGGING", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,   "JNI_DEBUGGING", __VA_ARGS__)
#else
extern Logger logger;

#define LOGV(...) logger.log(__FILE__,__FUNC__,__LINE__,ANDROID_LOG_VERBOSE, __VA_ARGS__)
#define LOGD(...) logger.log(__FILE__,__FUNC__,__LINE__,ANDROID_LOG_DEBUG, __VA_ARGS__)
#define LOGI(...) logger.log(__FILE__,__FUNC__,__LINE__,ANDROID_LOG_INFO, __VA_ARGS__)
#define LOGW(...) logger.log(__FILE__,__FUNC__,__LINE__,ANDROID_LOG_WARN, __VA_ARGS__)
#define LOGV(...) logger.log(__FILE__,__FUNC__,__LINE__,ANDROID_LOG_VERBOSE, __VA_ARGS__)
#define LOGE(...) logger.log(__FILE__,__FUNC__,__LINE__,ANDROID_LOG_ERROR, __VA_ARGS__)
#endif
using namespace std;

class ImgUtils {
public:
	static int dirExists(const char *path);
	static int createDir(const char * path);
	static int getFileCount(const char *path);
	static int getFileCount(const char *path,char *extn);
	static int getDirCount(char *dir);
	static int getFileExtn(char *name,char *extn);
	static vector<string> getSubDirs(char *path);

	static string getBaseDir(string file);
	static string getBaseName(string file);
};



#endif /* IMGUTILS_HPP_ */

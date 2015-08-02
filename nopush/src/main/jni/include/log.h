#ifndef _LOG_H_
#define _LOG_H_

#include<android/log.h>

#define LOG_TAG "NDK Log"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"%s",__VA_ARGS__)
#endif
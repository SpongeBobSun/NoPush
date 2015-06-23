#ifndef __JAVA_EXCEPTIONS__
#define __JAVA_EXCEPTIONS__

#define MAX_LOG_MESSAGE_LENGTH 1024

static void ThrowException(JNIEnv* env, const char* className,
        const char* message) {

    jclass clazz = env->FindClass(className);
    if (clazz != NULL) {
        env->ThrowNew(clazz, message);
        env->DeleteLocalRef(clazz);
    }
}

static void ThrowErrnoException(JNIEnv* env, const char* className,
        int errnum) {

    char buffer[MAX_LOG_MESSAGE_LENGTH];


    if (-1 == strerror_r(errnum, buffer, MAX_LOG_MESSAGE_LENGTH)) {
        strerror_r(errno, buffer, MAX_LOG_MESSAGE_LENGTH);
    }

    ThrowException(env, className, buffer);
}

#endif
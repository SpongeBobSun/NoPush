#include"sun_bob_nopush_NoPushService.h"

#include<signal.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<string.h>
#include<fcntl.h>
#include<sys/select.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>

#include<jni.h>

#include"include/log.h"

#define SIGCHECKDEAMON  50

char* InitSocketParams(JNIEnv* env, char* addr, int port);
void SingalHandler(int sigNo);
void Die(char* msg);
void performIO();
void sendStatusBroadcast();
void sendDaemonPid();

static struct itimerval value, oldValue;
static struct sockaddr_in server;
int socketFd = -1;
int processId = 0;
char* socketBuffer;
char* cmdBuffer;
char* broadcastMsgCmd = "am startservice --user 0 -a sun.bob.nopush.message -e rawData ";



JNIEXPORT jstring Java_sun_bob_nopush_NoPushService_entry(JNIEnv* env, jobject pThis, jstring server_addr, jint server_port, jint androidPid){
        signal(SIGALRM, SingalHandler);
        signal(SIGCHECKDEAMON,SingalHandler);
        value.it_interval.tv_sec = 3;
        value.it_interval.tv_usec = 0;
        value.it_value.tv_sec = 1;
        value.it_value.tv_usec = 0;

        if (fork() > 0){
            return 0;
        }
        processId = androidPid;
        char* errorStr = NULL;
        socketBuffer = (char*) malloc(1024);
        cmdBuffer = (char*) malloc(1024);
        jboolean isCopy;
        errorStr = InitSocketParams(env, (char* )(*env).GetStringUTFChars(server_addr,&isCopy),server_port);
        signal(SIGIO,SingalHandler);
        setitimer(ITIMER_REAL, &value, &oldValue);
        while(1){
            pause();
        }
    return (*env).NewStringUTF(errorStr);
}

char* InitSocketParams(JNIEnv* env, char* server_addr, int port){
    if ( (socketFd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        LOGE("Can not get socket fd");
        return strerror(errno);
    }

    memset(&server,0,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_addr);
    server.sin_port = htons(port);

    if (connect(socketFd, (struct sockaddr*) &server, sizeof(server)) < 0){
        LOGE("Can not connect server");
        LOGE(server_addr);
    }

    //Set socket to long lived. Amen.
    int enableVal = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_KEEPALIVE, &enableVal, sizeof(int)) < 0){
        LOGE("Can not set socket option");
    }

    //Set async IO.
    pid_t myPid = getpid(); //Get current pid first.
    int err;
    if ( (err = fcntl(socketFd, F_SETOWN, myPid) ) < 0 ) {  //Set socket fd's owner to current process.
            Die(strerror(err));
            LOGE("set own error");
        }
    if ( (err = ioctl(socketFd, FIOASYNC, &enableVal) ) < 0) {
            Die(strerror(err));
            LOGE("set async error");
        }
    sendDaemonPid();
    return NULL;
}
void SingalHandler(int sigNo){
  switch (sigNo){
    case SIGALRM:
      break;
    case SIGIO:
      performIO();
      break;
    case SIGCHECKDEAMON:
        LOGE("Check status.");
        break;
    default:
      break;
  }
}

void performIO(){
    fd_set readSet , writeSet;
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_SET(socketFd, &readSet);
    FD_SET(socketFd, &writeSet);

    select(socketFd+1, &readSet, &writeSet,NULL,NULL);

    if  (FD_ISSET(socketFd, &readSet)){
        //Read operaton
        memset(socketBuffer,'\0',1024*sizeof(char));
        recv(socketFd,socketBuffer,1024,0);
        char extras[1100];
        memset(extras,'\0',sizeof(char)*1100);
        strcat(extras,broadcastMsgCmd);
        strcat(extras,socketBuffer);
        LOGE(extras);
        system(extras);
        return;
    }
    if (FD_ISSET(socketFd, &writeSet)){
        //Write operation
        return;
    }
}

void sendDaemonPid(){
    char* cmd = "am startservice --user 0 -a sun.bob.nopush.daemon_pid -e pid %d sun.bob.nopushtest";
    memset(cmdBuffer,'\0',sizeof(char)*1024);
    sprintf(cmdBuffer,cmd,getpid());
    LOGE(cmdBuffer);
    system(cmdBuffer);
}

void sendStatusBroadcast(){

}
void Die(char* msg){
//  printf(msg);
//  exit(-1);
}
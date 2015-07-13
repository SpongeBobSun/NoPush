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

void InitSocketParams(char* addr, int port);
void SingalHandler(int sigNo);
void performIO();
void sendDaemonPid();
void heartbeat();

static struct itimerval value, oldValue;
static struct sockaddr_in server;
static bool socketConnected = false;

int socketFd = -1;
char* socketBuffer;
char* cmdBuffer;
char* broadcastMsgCmd = "am startservice --user 0 -a sun.bob.nopush.message -e rawData \'";

char* serverAddr;
int port;

char* UUID;
char* packageName;
char* heartbeatBuffer;

JNIEXPORT void Java_sun_bob_nopush_NoPushService_entry(JNIEnv* env, jobject pThis, jstring server_addr, jint server_port, jstring package, jstring uuid){
        signal(SIGALRM, SingalHandler);
        signal(SIGCHECKDEAMON,SingalHandler);
        value.it_interval.tv_sec = 60;
        value.it_interval.tv_usec = 0;
        value.it_value.tv_sec = 60;
        value.it_value.tv_usec = 0;

        //Convert parameters.
        jboolean isCopy;
        UUID = (char*) malloc(36);
        packageName = (char*) malloc(1024);
        memcpy(UUID,(*env).GetStringUTFChars(uuid,&isCopy),36);
        memset(packageName,'\0',1024);
        memcpy(packageName,(*env).GetStringUTFChars(package,&isCopy),1024);

        heartbeatBuffer = (char*)malloc(50);
        memset(heartbeatBuffer,'\0',50);
        memcpy(heartbeatBuffer,UUID,36);
        heartbeatBuffer[36] = 'H';

        if (fork() > 0){    //Parent process.
//            exit(0);
            //CAN NOT exit here. It will cause zygote process exit, AKA our service process.
            //Results in a BIG ANR to our service process. Damn!
            return;
        }
        if (setpgid(getpid(),(pid_t)1) != 0){
        	LOGE(strerror(errno));
        }
        //School out.

        char* errorStr = NULL;
        socketBuffer = (char*) malloc(1024);
        cmdBuffer = (char*) malloc(1024);

        //Save those params to global vars.
        serverAddr = (char* )(*env).GetStringUTFChars(server_addr,&isCopy);
        port = server_port;
        InitSocketParams(serverAddr,port);
        signal(SIGIO,SingalHandler);
        setitimer(ITIMER_REAL, &value, &oldValue);
        while(1){
            pause();
        }
    return;
}

void InitSocketParams(char* server_addr, int port){
    if ( (socketFd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        LOGE("Can not get socket fd");
        return;
    }

    memset(&server,0,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_addr);
    server.sin_port = htons(port);

    if (connect(socketFd, (struct sockaddr*) &server, sizeof(server)) < 0){
        LOGE("Can not connect server");
        socketConnected = false;
        return;
    }

    //Set socket to long lived. Amen.
    int enableVal = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_KEEPALIVE, &enableVal, sizeof(int)) < 0){
        LOGE("Can not set socket option");
        socketConnected = false;
        return;
    }

    //Set async IO.
    pid_t myPid = getpid(); //Get current pid first.
    int err;
    if ( (err = fcntl(socketFd, F_SETOWN, myPid) ) < 0 ) {  //Set socket fd's owner to current process.
            LOGE("set own error");
            socketConnected = false;
            return;
        }
    if ( (err = ioctl(socketFd, FIOASYNC, &enableVal) ) < 0) {
            LOGE("set async error");
            socketConnected = false;
            return;
        }
    sendDaemonPid();
    socketConnected = true;
    return;
}
void SingalHandler(int sigNo){
  switch (sigNo){
    case SIGALRM:
        heartbeat();
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

void heartbeat(){
    if (!socketConnected){
        InitSocketParams(serverAddr,port);
        if (!socketConnected)  //Even God can not save you either.
            return;
    }
    if (send(socketFd,heartbeatBuffer,50,0) < 0){
        LOGE("Send heartbeat error.");
        close(socketFd);
        socketConnected = false;
        return;
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
        int resultLen = strlen(socketBuffer);
        if(resultLen == 0)
            return;
        char extras[1100];
        memset(extras,'\0',sizeof(char)*1100);
        strcat(extras,broadcastMsgCmd);
        strcat(extras,socketBuffer);
        strcat(extras,"\'");
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
    char* cmd = "am startservice --user 0 -a sun.bob.nopush.daemon_pid -e pid %d ";
    memset(cmdBuffer,'\0',sizeof(char)*1024);
    sprintf(cmdBuffer,cmd,getpid());
    strcat(cmdBuffer,packageName);
    LOGE(cmdBuffer);
    system(cmdBuffer);
}

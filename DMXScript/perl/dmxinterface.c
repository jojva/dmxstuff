#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define DMX_MAX	512

#define true 1
#define false 0

/*global state vars*/
int *dmx_maxchannels;
int *dmx_shutdown;
int *dmx_caps;
int *dmx_channels;

int *shm;
int shmid;
bool connected;

void initDMX()
{
connected=false;
dmx_maxchannels=NULL;
dmx_shutdown=NULL;
dmx_caps=NULL;
dmx_channels=NULL;
shm=NULL;
shmid=-1;
}

bool dmxConnect()
{
    if(connected) return false;
    shmid=shmget(0x56444D58,sizeof(int)*522, 0666);
    shm=(int *)shmat(shmid,NULL,0);

    printf("k8062 startup: shmid=0x%x (%d) shm=0x%x (%d)\n",(unsigned int)shmid,shmid,(unsigned int)shm,shm);
    if(shm!=(int*)-1){
 	connected=true;
        dmx_maxchannels=shm;
   	dmx_shutdown=shm+1;
     	dmx_caps=shm+2;
    	dmx_channels=shm+10;
     }
     return connected;
}

bool dmxIsConnected()
{
return connected;
}

void dmxShutdown()
{
    *dmx_shutdown=1;
    connected=false;
    shmdt(shm);
}

bool setChannel(unsigned char ch,unsigned char n)
{
    if(ch>=0 && ch <=254 && n>=0 && n<=254){
	dmx_channels[ch]=n;
	return true;
    } else {
        return false;
    }
}

unsigned char peekChannel(unsigned char ch)
{
if(ch>=0 && ch <=254){
	return dmx_channels[ch];
}
return 0;
}


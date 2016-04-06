#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "k8062.h"

k8062_client::k8062_client()
{
    connected=false;

    connect();
}

bool k8062_client::connect()
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

k8062_client::~k8062_client()
{
    connected=false;
    shmdt(shm);
}

void k8062_client::shutdown()
{
    *dmx_shutdown=1;
    connected=false;
    shmdt(shm);
}

bool k8062_client::is_connected()
{
    return connected;
}

bool k8062_client::set_channel(BYTE ch,BYTE n)
{
    if(ch>=0 && ch <=254 && n>=0 && n<=254){
	dmx_channels[ch]=n;
	return true;
    } else {
        return false;
    }
}

BYTE k8062_client::peek_channel(BYTE ch)
{
if(ch>=0 && ch <=254){
	return dmx_channels[ch];
}
return 0;
}



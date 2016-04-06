#include <stdio.h>
#include <stdlib.h>

#include "k8062.h"

int main(int argc,char *argv[])
{
int max_channels=16;

class k8062_client dmx;
if(!dmx.is_connected()){
	puts("Error: Unable to connect to dmx daemon.\n");
	exit(1);
}

int cur_channel;

for(cur_channel=0;cur_channel<max_channels;++cur_channel){
	dmx.set_channel((BYTE)cur_channel,0);
}
}

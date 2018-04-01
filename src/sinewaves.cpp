#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

#include "k8062.h"

int main(void)
{
int max_channels=16;
int channel_offset=500;	/*the amount more, or less, that adjacent channels lag on the cycle*/
			/*this value is in milliseconds and is added to the current cycle
			timer when the channel value is calculated*/
long time_period=3500;	/*time in milliseconds, for a complete 0-1-0 cycle*/
long wait=25;		/*wait time in milliseconds*/
int monitor_channel=1;	/*print the values going to this channel number*/

puts("Velleman k8062 sinewaves demo, AMG 09\n\n");

class k8062_client dmx;

if(!dmx.is_connected()){
	puts("Error: Unable to connect to dmx daemon.\n");
	exit(1);
}

long n=0;
double v,a;
int output;
int cur_channel;

while(1){
	//a is the angle that we calculate sine of
	//2*M_PI radians=1 full cycle; however this is 2 full cycles for us as we don't
	//need the negative half-cycle and invert it. therefore the value we need is
	//half n/time_period times that.
	for(cur_channel=0;cur_channel<max_channels;++cur_channel){
		a=((double)(n+cur_channel*channel_offset)/(double)time_period)*(double)(M_PI);
		v=sin(a);
		output=(int)((double)v*(double)255);
		if(output<1){
			output=-output;
		}
		if(cur_channel==monitor_channel){
            printf("%ld - %f: %f (%d)\n",n,v,a,output);
		}

		dmx.set_channel((BYTE)cur_channel,(BYTE)output);
	}
	if(usleep(wait*1000)==-1){
		exit(2);
	}
	n+=wait;
}
}

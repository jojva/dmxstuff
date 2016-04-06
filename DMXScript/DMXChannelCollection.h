/*
 * DMXChannelCollection.h
 *
 *  Created on: 1 May 2010
 *      Author: andy
 */

#ifndef DMXCHANNELCOLLECTION_H_
#define DMXCHANNELCOLLECTION_H_

#define MAX_CHANNELS 512

typedef class DMXChannelCollection {
public:
	DMXChannelCollection();
	virtual ~DMXChannelCollection();

	void setChannelController(uint8 chl,class channelController *ctrl);
	void unsetChannel(uint8 chl);
	void setChannelDirect(uint8 chl,uint8 val);

	void clock();

private:
	class channelController *channel[MAX_CHANNELS];

	class k8062Client dmxClient;

} DMXChannelCollection;

#endif /* DMXCHANNELCOLLECTION_H_ */

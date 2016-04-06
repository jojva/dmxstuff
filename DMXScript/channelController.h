/*
 * channelController.h
 *
 *  Created on: 1 May 2010
 *      Author: andy
 */

#ifndef CHANNELCONTROLLER_H_
#define CHANNELCONTROLLER_H_

class channelController {
public:
	channelController();
	channelController(char *libraryName);

	virtual ~channelController();

	uint8 getValue();
	void clock();
private:
	class DMXChannelCollection *parentCollection;

	void (*getValueLibrary)(int t);
	void (*clockLibrary)();
};

#endif /* CHANNELCONTROLLER_H_ */

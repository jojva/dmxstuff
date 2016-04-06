#include <iostream>
#include <string>

#include "linkedlist.h"
#include "DMXChannelCollection.h"
#include "channelController.h"
#include "controllerCollection.h"
using namespace std;

#define DEBUG

/*
 * parser.cpp
 *
 *  Created on: 1 May 2010
 *      Author: andy
 */

int parseExpression(string *expr,)
int parseScriptFile(char *filename,DMXChannelCollection *chls,controllerCollection *ctrls)
{
	ifstream fp;
	string line,dmx,expr;
	size_t delim;
	char *dmxStr;
	uint8 nDMX;

	if(!filename){
			cerr << "parseScriptFile - no filename given.\n";
			return -1;
	}

	fp.open(filename);
	if(!fp.is_open()){
		cerr << "Unable to open script file '"<<filename<<"'.\n";
		return -2;
	}

	while(!fp.eof()){
		//each line should be of the format dmx_{n}={expr}. Lines beginning with # or empty are ignored.
		getline(fp,line);
#ifdef DEBUG
		cerr << "\tparseScriptFile: got line "<<line<<".\n";
#endif
		if(line.size>3 && line[0]!='#'){
			delim=line.find('=');
			dmx=line.substr(0,delim-1);
			expr=line.substr(delim+1);	/*leave out 2nd arg=> go to end*/
			dmxStr=dmx.c_str();
#ifdef DEBUG
		cerr << "\tparseScriptFile: dmx part "<<dmx<<", expression part "<<expr<<".\n";
#endif
			if(!dmxStr){
				cerr << "parseScriptFile - something weird happened.\n";
				abort();
			}
			sscanf(dmxStr,"dmx_%d",&nDMX);
#ifdef DEBUG
		cerr << "\tparseScriptFile: got dmx channel "<<nDMX<<".\n";
#endif
		}
	}
}

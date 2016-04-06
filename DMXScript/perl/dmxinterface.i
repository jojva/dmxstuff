%module dmxinterface
%{
typedef unsigned char bool;
%}

void initDMX();
unsigned char dmxConnect();
unsigned char dmxIsConnected();
void dmxShutdown();
unsigned char setChannel(unsigned char ch,unsigned char n);
unsigned char peekChannel(unsigned char ch);


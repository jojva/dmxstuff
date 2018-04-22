#include "synesthesizer.h"

#include <iostream>
#include <signal.h>

void sig_handler(int signum)
{
    std::cout << "Ctrl+C received. Exiting synesthesizer." << std::endl;
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_handler);
    CSynesthesizer Synesthesizer;
    Synesthesizer.Init(argc, argv);
    while(true)
    {
        Synesthesizer.Run();
    }
}

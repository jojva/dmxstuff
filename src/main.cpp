#include "synesthesizer.h"

int main(int argc, char *argv[])
{
    CSynesthesizer Synesthesizer;
    Synesthesizer.Init(argc, argv);
    while(true)
    {
        Synesthesizer.Run();
    }
}

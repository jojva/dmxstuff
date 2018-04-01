#include "synesthesizer.h"

int main(int aArgc,char *aArgv[])
{
    CSynesthesizer Synesthesizer;
    Synesthesizer.Init(aArgc, aArgv);
    Synesthesizer.Run();
    return 0;
}

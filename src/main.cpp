#include <iostream>
#include "Emulator/Emulator.h"

Emulator emulator;

int main(int argc, char *argv[])
{

    if(argc >= 2)
    {
        if(emulator.load(argv[1]))
        {
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error: No ROM file specified.\n");
        return 1;
    }
    
    if(emulator.init())
    {
        return 1;
    }

    emulator.start();
    while (1)
    {
        
    }
    emulator.stop();
    return 0;
}
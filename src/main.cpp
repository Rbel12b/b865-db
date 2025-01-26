#include <iostream>
#include "Parser/Parser.h"

Parser parser;

int main(int argc, char *argv[])
{

    if(argc >= 2)
    {
        DebuggerData* data = parser.parse(argv[1]);
        if(data != NULL)
        {
            printf("Modules:\n");
            for(auto& str : data->modules)
            {
                printf("%s\n", str.c_str());
            }
        }
        delete data;
    }
    return 0;
}
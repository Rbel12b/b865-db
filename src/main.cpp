#include <iostream>
#include "Parser/cdbParser.h"

cdbParser parser;

int main(int argc, char *argv[])
{

    if(argc >= 2)
    {
        parser.init(argv[1]);

        DebuggerData* data = parser.parse();
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
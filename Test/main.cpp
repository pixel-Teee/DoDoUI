#include <Application/Application.h>

#include "Core/String/DoDoString.h"

#include <fstream>

int main(int argc, char* argv[])
{
    if (argc < 2 || argv[1] == std::string("1"))
    {
        DoDo::DoDoUtf8String str("🐟");

        //5 = 4 + 1
        if (str.get_count() != 5 && str.get_length() != 1)
            return -1;

        //std::ofstream f("test.txt");
        //f << str.c_str();
        //f.close();
        return 0;
    }
    else if(argc < 2 || argv[1] == std::string("2"))
    {
        DoDo::DoDoUtf8String str("过一天是一天，努力奋斗！upup！");

        if (str.get_count() != 44 && str.get_length() != 18)
            return -1;
        return 0;
    }

    return 0;
}

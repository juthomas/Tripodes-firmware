#include <stdio.h>
#include <iostream>

int main(int argc, char **argv)
{
    std::string inputString(argv[1]);
    std::cout << inputString << "\n";
    int index = 0;
    int tmp1;
    int tmp2;
    while ((tmp1 = inputString.find("{")) != -1)
    {
        if ((tmp2 = inputString.find("}", tmp1)) != -1)
        {
            std::string modifystring;
            modifystring = inputString.substr(tmp1, tmp2 - tmp1 + 1);
            std::cout << "tmp1 :" << tmp1 << "   tmp2 :" << tmp2 << "\n";
            std::cout << "Omg nice:" << modifystring << "\n";
            modifystring = modifystring.substr(1, modifystring.length() - 2);
            std::cout << "Omg nice2:" << modifystring << "\n";
            inputString.replace(tmp1, tmp2 - tmp1 + 1 , modifystring);
            std::cout << "Input str:" << inputString << "\n\n";
        }
        else
        {
            break;
        }
    }
    std::cout << tmp1 << "\n";
    std::cout << tmp2 << "\n";

    // if ( (inputString.find('{')) )
}
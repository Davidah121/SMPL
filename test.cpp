#include <iostream>

std::string s = "";
const std::string constS = "";

std::string& getS(bool v)
{
    if(v)
        return s;
    else
        return constS;
}

int main()
{
    std::cout << s << std::endl;
    std::string temp = getS(true);
    std::string temp2 = getS(false);
    std::cout << s << std::endl;

}
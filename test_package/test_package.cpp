#include <fplus/fplus.hpp>
#include <iostream>

int main()
{
    std::list<std::string> things = {"same old", "same old"};
    if (fplus::all_the_same(things))
        std::cout << "All things being equal." << std::endl;
}
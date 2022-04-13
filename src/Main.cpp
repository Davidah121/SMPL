#include "StringTools.h"
#include "System.h"
#include "SimpleWindow.h"

using namespace glib;

void testLinuxStuff()
{
    StringTools::init();
    SimpleWindow w = SimpleWindow("Windows Ver", 640, 480);
    w.waitTillClose();
}

int main(int argc, char** argv)
{
    testLinuxStuff();
    return 0;
}

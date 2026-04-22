#include <string>
#include "gui.h"

namespace gui
{
void init()
{
    binds::initBinds();
}

void destroy()
{
    binds::destroyBinds();
}

Menu& getMenuInstance()
{
    static Menu instance;
    return instance;
}
}
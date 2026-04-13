#include "gui.h"

namespace gui
{
void init()
{

}

void destroy()
{

}

Menu& getMenuInstance()
{
    static Menu instance;
    return instance;
}
}
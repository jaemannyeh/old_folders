

#include <iostream>

#include "pp_class.h"

using namespace std;

extern "C" void pp_class_init()
{
    cout << __FUNCTION__ << '(' << __LINE__ << ')' << ' ' << __cplusplus << '\n';

}

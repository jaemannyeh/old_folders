
/* 1 description ******************************************/

/* 2 configuration ****************************************/

/* 3 include files ****************************************/

#include "oe_common.h"
#include "oe_once.h"

/* 4 defines **********************************************/

/* 5 types ************************************************/

/* 6 extern prototypes ************************************/

/* 7 local prototypes *************************************/

/* 8 data *************************************************/

OEOnce  TheOnce;
OEOnce *theOnce = NULL;

/* 9 local functions **************************************/

/* 10 public functions ************************************/

OEOnce::OEOnce()
{
    if (theOnce != NULL)
        return;

    theOnce = this;
}

OEOnce::~OEOnce()
{
}


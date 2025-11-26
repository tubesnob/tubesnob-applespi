#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "w5500.h"
#include "w5500_defs.h"
#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tsiplib/src/tsiplib.h"
#include "../../orcadefaults.h"

#ifndef __TSIPLIB_W5500_DRIVER_DOT_H__
#define __TSIPLIB_W5500_DRIVER_DOT_H__

bool tsiplib_w5500_driver_init();
net_driver_t* tsiplib_w5500_driver_create();

#endif

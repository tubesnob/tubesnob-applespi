#include <stdio.h>
#include <stdint.h>

#include "../../orcadefaults.h"
#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tssocketlib/src/tssocketlib.h"
#include "../../tssocketlib/src/tssocketlib_w5500.h"

#ifndef __TSSOCKETLIB_TEST_DOT_H__
#define __TSSOCKETLIB_TEST_DOT_H__

void dump_socketStatus(socket_t* socket);

#endif
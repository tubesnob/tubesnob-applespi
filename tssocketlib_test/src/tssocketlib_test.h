#include <stdio.h>

#include "../../tslib/lib/tslib.h"
#include "../../tsspilib/lib/tsspilib.h"
#include "../../tssocketlib/lib/tssocketlib.h"
#include "../../tssocketlib/lib/tssocketlib_w5500.h"

#ifndef __TSSOCKETLIB_TEST_DOT_H__
#define __TSSOCKETLIB_TEST_DOT_H__

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int

void dump_socketStatus(socket_t* socket);


#endif
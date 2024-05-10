#include <stdio.h>
#include <stdint.h>

#include "../../orcadefaults.h"
#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tssocketlib/src/tssocketlib.h"
#include "../../tssocketlib/src/tssocketlib_w5500.h"


#ifdef __MACOS__
#include "../../tsspilib_driver_ftdi/src/tsspilib_driver_ftdi.h"
#endif

#ifdef __APPLE2GS__
#include "../../tsspilib_driver_a2gpio/src/tsspilib_driver_a2gpio.h"
#endif

#ifndef __TSSOCKETLIB_TEST_DOT_H__
#define __TSSOCKETLIB_TEST_DOT_H__

int initializeSPI();
int initializeW5500();
int initializeSockets();
int initializeLWIP(socket_t* socket);

void dump_socketStatus(socket_t* socket);
uint32_t lwip_port_rand();
uint32_t sys_now();
int intmin(int x, int y);

static uint8_t _hwaddr[]   = { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33 };
static uint8_t _ipaddr[]   = { 10, 0, 0, 133 };
static uint8_t _mask[]     = { 255, 255, 255, 0 };
static uint8_t _gwaddr[]   = { 10, 0, 0, 1};

static tsspilib_device_vtbl_t* _spiDevice = NULL;


#endif
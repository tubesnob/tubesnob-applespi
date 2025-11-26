#include <stdio.h>
#include <stdint.h>

#include "../../orcadefaults.h"
#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tsiplib/src/tsiplib.h"
#include "../../tsiplib_driver_w5500/src/tsiplib_w5500_driver.h"

#ifndef __TSIPLIB_TEST_DOT_H__
#define __TSIPLIB_TEST_DOT_H__

/* Function to process raw W5500 MACRAW data and extract Ethernet frames */
void process_w5500_macraw_data(const uint8_t *raw_data, uint16_t data_len);

#endif
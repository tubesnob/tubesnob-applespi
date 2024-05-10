#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "lwip_test.h"

unsigned long lwip_port_rand()
{
  return (uint32_t)rand();
}

unsigned long sys_now() {
    return clock();
}

int intmin(int x, int y) {
  return (x < y) ? x : y;
}





  
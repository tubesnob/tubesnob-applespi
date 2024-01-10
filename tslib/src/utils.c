#include "tslib.h"
#include "../../orcadefaults.h"
#pragma noroot

#define __debug_log_buffer_size 0x100
time_t __debug_log_time_val;
clock_t __debug_log_clock_val;
struct tm*  __debug_log_local_time;
char *__debug_log_asctime;
char __debug_buffer[__debug_log_buffer_size];

void waitMilliseconds(int ms)
{
   int ticksNeeded = ms / 16.666f;
   clock_t start = clock();
   clock_t end = clock();
   do {
      end = clock();
   }
   while (end-start < ticksNeeded);
}

void waitSeconds(int seconds)
{

   waitMilliseconds(seconds*1000);
/*
   time_t tStart;
   time_t tEnd;
   double diff = 0;
   time(&tStart);
   while(diff < seconds) {
      time(&tEnd);
      diff = difftime(tEnd, tStart);
   }
*/
}

static inline void update_debug_time() {
   __debug_log_clock_val = clock();
   time(&__debug_log_time_val);
   __debug_log_local_time = localtime(&__debug_log_time_val);
   struct tm* t = __debug_log_local_time;
   sprintf(__debug_buffer,"%04d%02d%02d-%02d%02d%02d.%02d\0",t->tm_year==118?2018:t->tm_year,t->tm_mon,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,(unsigned int)(__debug_log_clock_val%60));
}

//void DEBUG_LOG(const char *fmt)
//{
      //update_debug_time();
      //va_list args;
      //va_start(args, fmt);
      //printf("[LOG][%s]:",__debug_buffer);
      //vprintf(fmt,args);
      //va_end(args);
//      printf("%s",fmt);
//}

char *strdup(const char *s) {
    if (!s) 
        return (char*)s;
    int l = strlen(s);
    char *rv = (char*) malloc(l+1);
    memset(rv,0,l+1);
    strcpy(rv, s);
    return rv;
}

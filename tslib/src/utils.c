#include "tslib.h"
#include "../../orcadefaults.h"

#define __debug_log_buffer_size 0x100
static time_t __debug_log_time_val;
static clock_t __debug_log_clock_val;
static struct tm*  __debug_log_local_time;
static char *__debug_log_asctime;
static char __debug_buffer[__debug_log_buffer_size];

void tslib_init() {
    tslog_init();
    tsarray_init();
    tsmem_init();
    tsstring_init();
    tslist_init();
}

void tslib_shutdown() {
    tslog_shutdown();
    tsarray_shutdown();
    tsstring_shutdown();
    tslist_shutdown();
    tsmem_shutdown();
}

void waitMilliseconds(uint32_t ms)
{
   uint32_t ticksNeeded = ms / 16.666f;
   clock_t start = clock();
   clock_t end = clock();
   do {
      end = clock();
   }
   while (end-start < ticksNeeded);
}

void waitSeconds(uint32_t seconds)
{
   waitMilliseconds(seconds*1000);
}

static inline void update_debug_time() {
   __debug_log_clock_val = clock();
   time(&__debug_log_time_val);
   __debug_log_local_time = localtime(&__debug_log_time_val);
   struct tm* t = __debug_log_local_time;
   sprintf(__debug_buffer,"%04i%02i%02i-%02i%02i%02i.%02i\0",t->tm_year==118?2018:t->tm_year,t->tm_mon,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,(unsigned int)(__debug_log_clock_val%60));
}

char *strdup(const char *s) {
    if (!s) 
        return (char*)s;
    int l = strlen(s);
    char *rv = (char*) malloc(l+1);
    memset(rv,0,l+1);
    strcpy(rv, s);
    return rv;
}

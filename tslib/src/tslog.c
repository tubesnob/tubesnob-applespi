#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "tslib.h"
#include "../../orcadefaults.h"

tslog_vtbl_t* _tslog;

static void  tslog_error(const char *fmt, ...);
static void  tslog_verbose(const char *fmt, ...);
static void  tslog_info(const char *fmt, ...);
static void  tslog_debug(const char *fmt, ...);

tslog_vtbl_t* tslog_init() {
    if (!_tslog) {
        tslog_vtbl_t* rv = (tslog_vtbl_t*) malloc(sizeof(tslog_vtbl_t));
        rv->error = &tslog_error;
        rv->info = &tslog_info;
        rv->verbose = &tslog_verbose;
        rv->debug = &tslog_debug;
        rv->logMask = TSLOG_LEVEL_ALL;
        rv->buffer = (char*) malloc(0x400);
        return _tslog = rv;
    }
    return _tslog;
}

void tslog_shutdown() {
    freeandnull(_tslog->buffer);
    freeandnull(_tslog);
}

static void  tslog_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (_tslog->logMask & TSLOG_LEVEL_ERROR) {
        vfprintf(stderr, fmt, args);
    }
    else
    {
        vsprintf(_tslog->buffer, fmt, args);
    }
    va_end(args);
}

static void  tslog_verbose(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (_tslog->logMask & TSLOG_LEVEL_VERBOSE)
    {
        vfprintf(stderr, fmt, args);
    }
    else
    {
        vsprintf(_tslog->buffer, fmt, args);
    }
    va_end(args);
}

static void  tslog_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (_tslog->logMask & TSLOG_LEVEL_INFO) {
        vfprintf(stderr, fmt, args);
    }
    else
    {
        vsprintf(_tslog->buffer, fmt, args);
    }
    va_end(args);
}

static void  tslog_debug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (_tslog->logMask & TSLOG_LEVEL_DEBUG) {
        vfprintf(stderr, fmt, args);
    }
    else
    {
        vsprintf(_tslog->buffer, fmt, args);
    }
    va_end(args);
}



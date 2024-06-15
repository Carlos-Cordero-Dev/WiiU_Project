#pragma once

#include <cstdio>
#include <cstdarg>

#include <whb/log.h>

#define VORP_LOG(fmt, ...) vorp_log(fmt, ##__VA_ARGS__)

// Logging function
void vorp_log(const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    WHBLogPrintf("%s", buffer);
}

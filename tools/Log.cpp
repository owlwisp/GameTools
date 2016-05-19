// 分级日志系统

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

enum LoggingLevel {
    LOGLEVEL_FATAL = -1,
    LOGLEVEL_ERROR = 0,
    LOGLEVEL_INFO  = 1,
    LOGLEVEL_DEBUG = 2,
    LOGLEVEL_DEBUG_V1 = 3,
    LOGLEVEL_DEBUG_V2 = 4,
    LOGLEVEL_DEBUG_V3 = 5,
    LOGLEVEL_TRACE = 6
};

int64_t GetCurrentTime();
void SetLoggingLevel(int level);
int GetLoggingLevel();

void TraceLog(const char* format, ...);
void DebugV3Log(const char* format, ...);
void DebugV2Log(const char* format, ...);
void DebugV1Log(const char* format, ...);
void DebugLog(const char* format, ...);
void InfoLog(const char* format, ...);
void ErrorLog(const char* format, ...);
void FatalLog(const char* format, ...);


static int log_level = LOGLEVEL_ERROR;
static const size_t BUF_LEN = 8192;

void SetLoggingLevel(int level) {
    log_level = level;
}

int GetLoggingLevel() {
    return log_level;
}


static void PrefixLog(const char* prefix, const char* format, va_list args) {
    char buf[BUF_LEN];
    int64_t now = GetCurrentTime();
    time_t ts = now / 1000;
    int ms = now % 1000;
    struct tm* tm_now = localtime(&ts);

    sprintf(buf, "[%s][%04d-%02d-%02d %02d:%02d:%02d.%03d] ",
            prefix, tm_now->tm_year + 1900, tm_now->tm_mon + 1,
            tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec,
            ms);
    size_t prefix_len = strlen(buf);
    vsnprintf(buf + prefix_len, BUF_LEN - prefix_len, format, args);
    printf("%s\n", buf);
}
    
void TraceLog(const char* format, ...) {
    if (log_level >= LOGLEVEL_TRACE) {
        va_list args;
        va_start(args, format);
        PrefixLog("TRACE", format, args);
        va_end(args);
    }
}

void DebugV3Log(const char* format, ...) {
    if (log_level >= LOGLEVEL_DEBUG_V3) {
        va_list args;
        va_start(args, format);
        PrefixLog("DEBUG", format, args);
        va_end(args);
    }
}

void DebugV2Log(const char* format, ...) {
    if (log_level >= LOGLEVEL_DEBUG_V2) {
        va_list args;
        va_start(args, format);
        PrefixLog("DEBUG", format, args);
        va_end(args);
    }
}

void DebugV1Log(const char* format, ...) {
    if (log_level >= LOGLEVEL_DEBUG_V1) {
        va_list args;
        va_start(args, format);
        PrefixLog("DEBUG", format, args);
        va_end(args);
    }
}

void DebugLog(const char* format, ...) {
    if (log_level >= LOGLEVEL_DEBUG) {
        va_list args;
        va_start(args, format);
        PrefixLog("DEBUG", format, args);
        va_end(args);
    }
}

void InfoLog(const char* format, ...) {
    if (log_level >= LOGLEVEL_INFO) {
        va_list args;
        va_start(args, format);
        PrefixLog("INFO", format, args);
        va_end(args);
    }
}

void ErrorLog(const char* format, ...) {
    if (log_level >= LOGLEVEL_ERROR) {
        va_list args;
        va_start(args, format);
        PrefixLog("ERROR", format, args);
        va_end(args);
    }
}

void FatalLog(const char* format, ...) {
    if (log_level >= LOGLEVEL_FATAL) {
        va_list args;
        va_start(args, format);
        PrefixLog("FATAL", format, args);
        va_end(args);
    }
}
    
int64_t GetCurrentTime(){
    int64_t current_time = 0;
#if __cplusplus >= 201103 || _MSC_VER >= 1800
    auto tp = std::chrono::system_clock::from_time_t(0);
    auto now = std::chrono::system_clock::now();
    auto d = std::chrono::duration_cast<std::chrono::milliseconds>(now - tp);
    current_time = d.count();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    current_time = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif

    return current_time;
}


int main(){
    SetLoggingLevel(LOGLEVEL_TRACE);
    DebugLog("%s","This is Debug log 1");
    SetLoggingLevel(LOGLEVEL_INFO);
    DebugLog("%s","This is Debug log 2");
}
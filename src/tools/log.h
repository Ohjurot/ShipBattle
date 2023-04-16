#pragma once

#include "logger.h"

inline Logger** GetLoggerPP()
{
    static Logger* ptr = nullptr;
    return &ptr;
}

inline Logger& GetLogger()
{
    return **GetLoggerPP();
}

inline void SetLogger(Logger& logger)
{
    *GetLoggerPP() = &logger;
}

#pragma once

#include <string_view>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

#include <Shlobj.h>

class Logger
{
    public:
        Logger();

        Logger& log(std::string_view text);
        Logger& log(HRESULT hr);

    private:
        std::fstream m_logfile;
};

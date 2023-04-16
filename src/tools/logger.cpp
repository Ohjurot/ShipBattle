#include "pch.h"
#include "logger.h"

Logger::Logger()
{
    char appdatapath[MAX_PATH] = {'\0'};
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appdatapath);

    std::time_t t = std::time(nullptr);
    std::tm time;
    localtime_s(&time, &t);

    std::stringstream ss;
    ss << appdatapath << "\\ShipBattle\\" << std::put_time(&time, "%d_%m_%Y-%H_%M_%S") << ".log";

    std::filesystem::path path = ss.str();
    std::filesystem::path dir = path;
    dir.remove_filename();
    
    std::filesystem::create_directories(dir);
    m_logfile.open(path, std::ios::out);
}

Logger& Logger::log(std::string_view text)
{
    m_logfile << text << std::endl;
    m_logfile.flush();
    return *this;
}

Logger& Logger::log(HRESULT hr)
{
    char* errorText;
    if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorText, 0, nullptr) != 0)
    {
        log(errorText);
        LocalFree(errorText);
    }
    return *this;
}

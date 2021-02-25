#include "util/cmd.h"
#include "util/util.h"

#include <cstdarg>
#include <cstdio>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <string.h>

#ifndef LOG_H
#define LOG_H

template <typename T>
void string_builder(std::stringstream &s, const char *sep, T t)
{
    s << t << sep;
}

template <typename T, typename... Args>
void string_builder(std::stringstream &s, const char *sep, T t, Args... args)
{
    s << t << sep;

    string_builder(s, sep, args...);
}

template <typename T, typename... Args>
std::string string_builder(const char *sep, T t, Args... args)
{
    std::stringstream s;

    string_builder(s, sep, t, args...);
    std::string str = s.str();

    return str.substr(0, str.size() - strlen(sep));
}

std::string string_builder(const char *sep);

template <typename... Args>
void log(char **argv, char **argv_end, Args... args)
{
    std::string str = string_builder("", args...) + "\n";

    char *log_file = get_cmd_option(argv, argv_end, "-log");

    if (log_file)
    {
        std::fstream file;
        file.open(log_file, std::ios::out | std::ios::app);

        if (not file.is_open())
        {
            return;
        }

        str = get_system_time() + ": " + str;

        file.write(str.data(), str.size());
        file.close();
    }
}

#endif
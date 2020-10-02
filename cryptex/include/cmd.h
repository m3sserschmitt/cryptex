#include <string>

char* get_cmd_option(char ** begin, char ** end, const std::string & option);
bool cmd_option_exists(char** begin, char** end, const std::string& option);

#ifndef CMD_H
#define CMD_H

template <typename T>
bool cmd_one_exists(char **begin, char **end, T t) {
    return cmd_option_exists(begin, end, t);
}

template <typename T, typename ...Args>
bool cmd_one_exists(char **begin, char **end, T t, Args ...args) {
    if(cmd_option_exists(begin, end, t)) {
        return true;
    }

    return cmd_one_exists(begin, end, args...);
}

template <typename ...Args>
bool cmd_one_exists(char **begin, char **end, Args ...args) {
    return cmd_one_exists(args...);
}

#endif
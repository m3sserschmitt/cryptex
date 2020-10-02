#include <vector>
#include <string>
#include <string.h>
#include <sstream>

std::vector<std::string> split(std::string str, std::string sep, int max_split);

#ifndef PARSE_H
#define PARSE_H

template <typename T>
void add_command(char **argv, int &argc, T t)
{
    std::vector<std::string> tokens = split(t, " ", -1);

    size_t size = tokens.size();
    size_t i = 0;

    for (; i < size; i++)
    {
        argv[i + argc] = (char *)malloc(tokens[i].size() + 1);
        strcpy(argv[i + argc], tokens[i].c_str());
    }

    argc += i;
}

template <typename T, typename... Args>
void add_command(char **argv, int &argc, T t, Args... args)
{
    std::vector<std::string> tokens = split(t, " ", -1);

    size_t size = tokens.size();
    size_t i = 0;

    for (; i < size; i++)
    {
        argv[i + argc] = (char *)malloc(tokens[i].size() + 1);
        strcpy(argv[i + argc], tokens[i].c_str());
    }

    argc += i;

    add_command(argv, argc, args...);
}

template <typename... Args>
void parse_command(char **argv, int &argc, Args... args)
{
    argc = 0;
    add_command(argv, argc, args...);
}

#endif

std::string get_system_time();
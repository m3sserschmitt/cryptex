#include "util/util.h"

#include <string.h>
#include <iomanip>

std::vector<std::string> split(std::string str, std::string sep, int max_split)
{
    std::vector<std::string> tokens;
    size_t sep_pos;
    int split_index = 0;

    if (!str.size())
        return tokens;

    do
    {
        split_index++;
        sep_pos = str.find(sep);

        tokens.push_back(str.substr(0, sep_pos));

        if (sep_pos == std::string::npos)
            return tokens;

        str = str.substr(sep_pos + sep.size());

        if (split_index == max_split && str.size())
        {
            tokens.push_back(str);
            return tokens;
        }

    } while (true);

    return tokens;
}

std::string get_system_time()
{
    time_t t = time(nullptr);
    tm _tm = *localtime(&t);
    std::stringstream ss;

    ss << std::put_time(&_tm, "%a,%e %b %G %T %Z");

    return ss.str();
}

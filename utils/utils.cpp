#include <vector>
#include <string>
#include <fstream>

#include "utils.h"

std::vector<std::string> readlines(std::fstream fp)
{    
    std::vector<std::string> lines;
    std::string line;
    while(getline(fp, line))
    {
        lines.push_back(line);
    }
    return lines;
}

std::string trim_whitespaces(const std::string& str, const std::string& whitespace = " \t\n\r")
{
    const auto str_begin = str.find_first_not_of(whitespace);

    if (str_begin == std::string::npos)
        return ""; // no content

    const auto str_end = str.find_last_not_of(whitespace);
    const auto str_length = str_end - str_begin + 1;

    return str.substr(str_begin, str_length);
}
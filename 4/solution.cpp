#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

std::vector<std::string> readlines(std::fstream& fp)
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

typedef pair<uint32_t, uint32_t> range_s;
typedef pair<range_s, range_s> line_data_s;
typedef pair<string, string> split_line_s;

split_line_s split_string(string line, char sep)
{
    uint32_t sep_index = line.find_first_of(sep);

    if(sep_index == string::npos)
    {
        return split_line_s(line, "");
    }

    uint32_t part1_len = sep_index, part1_begin = 0;
    uint32_t part2_len = line.length() - (sep_index + 1), part2_begin = (sep_index + 1);
    string part1 = line.substr(part1_begin, part1_len);
    string part2 = line.substr(part2_begin, part2_len);

    return split_line_s(part1, part2);
}

line_data_s get_line_data(const string& line)
{
    string line_copy;
    line_copy = trim_whitespaces(line);
    // example: 8-98,7-97
    // split line to 2 substrings
    split_line_s comma_split = split_string(line_copy, ',');

    // split each part 
    split_line_s part1_split = split_string(comma_split.first, '-');
    split_line_s part2_split = split_string(comma_split.second, '-');
    
    // string to int
    line_data_s result;
    result.first  = range_s(stoi(part1_split.first), stoi(part1_split.second));
    result.second = range_s(stoi(part2_split.first), stoi(part2_split.second));

    return result;
}

bool are_groups_overlapping(line_data_s ranges, bool full_overlap_only = true)
{
    if(full_overlap_only)
    {
        bool is_first_in_second = ranges.second.first <= ranges.first.first && ranges.first.second <= ranges.second.second;
        bool is_second_in_first = ranges.first.first <= ranges.second.first && ranges.second.second <= ranges.first.second;
        
        return is_first_in_second || is_second_in_first;
    }
    else
    {
        bool is_first_left_in_second = (ranges.second.first <= ranges.first.first && ranges.first.first <= ranges.second.second);
        bool is_first_right_in_second = (ranges.second.first <= ranges.first.second && ranges.first.second <= ranges.second.second);

        bool is_second_left_in_first = (ranges.first.first <= ranges.second.first && ranges.second.first <= ranges.first.second);
        bool is_second_right_in_first = (ranges.first.first <= ranges.second.second && ranges.second.second <= ranges.first.second);

        return is_first_left_in_second || is_first_right_in_second || is_second_left_in_first || is_second_right_in_first;
    }
}

int main()
{
    fstream fp;
    fp.open("input.txt", ios::in);
    vector<string> lines = readlines(fp);
    for(uint32_t part = 0; part < 2; part++)
    {
        cout << "part " << part + 1 << ", count = ";
        uint32_t counter = 0;
        bool full_overlap_flag = (part == 0);
        for(uint32_t i = 0; i < lines.size(); i++)
        {
            line_data_s data = get_line_data(lines.at(i));
            if(are_groups_overlapping(data, full_overlap_flag))
                counter++;
        }
        cout << counter << endl;
    }
}

void test_get_line_data()
{
    line_data_s test = get_line_data("8-98,7-97");
    cout << test.first.first << " " << test.first.second << " " << test.second.first << " " << test.second.second << endl;
}

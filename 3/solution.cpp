/*
first task

priority: a = 1, b = 2, ..., z = 26, A = 27, ..., Z = 52

read the file, 
split each line into 2 halves,
find the common item for each 2 halves,
sum the priorities of each common item
*/

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define HISTOGRAM_SIZE 53

vector<string> load_file(string path)
{
    fstream fp;
    fp.open(path, ios::in);
    string line;
    vector<string> lines;
    while(getline(fp, line))
    {
        lines.push_back(line);
    }
    return lines;
}

uint32_t get_priority(char letter)
{
    uint32_t priority = 0;
    if(letter >= 'a' && letter <= 'z')
        priority = (letter - 'a') + 1;
    else if(letter >= 'A' && letter <= 'Z')
        priority = (letter - 'A') + 27;
    else
    {
        cout << "INVALID PARAMETER: " << letter << endl;
        throw;
    }
    return priority;
}


string trim(const string& str, const string& whitespace = " \t\n\r")
{
    const auto str_begin = str.find_first_not_of(whitespace);

    if (str_begin == string::npos)
        return ""; // no content

    const auto str_end = str.find_last_not_of(whitespace);
    const auto str_length = str_end - str_begin + 1;

    return str.substr(str_begin, str_length);
}

uint32_t get_common_letter_priority(string line1, string line2, string line3 = "")
{
    // create 2 histogram arrays instead of iterating the strings in 2 for loops - o(n) instead of o(n^2)
    bool letter_histogram1[HISTOGRAM_SIZE] = {0}, letter_histogram2[HISTOGRAM_SIZE] = {0}, letter_histogram3[HISTOGRAM_SIZE] = {0};
    // fill the histograms
    // treat the letter priority like an index in the array [index 0 will not be used]

    line1 = trim(line1);
    line2 = trim(line2);
    line3 = trim(line3);
    for(uint32_t i = 0; i < line1.length(); i++)
    {
        uint32_t histogram_index = get_priority(line1.at(i));
        letter_histogram1[histogram_index] = true;
    }
    for(uint32_t i = 0; i < line2.length(); i++)
    {
        uint32_t histogram_index = get_priority(line2.at(i));
        letter_histogram2[histogram_index] = true;
    }
    for(uint32_t i = 0; i < line3.length(); i++)
    {
        uint32_t histogram_index = get_priority(line3.at(i));
        letter_histogram3[histogram_index] = true;
    }
    // find the common priority and return it
    for(uint32_t i = 0; i < HISTOGRAM_SIZE; i++)
    {
        if(letter_histogram1[i] && letter_histogram2[i] && (line3.length() == 0 || letter_histogram3[i]))
            return i;
    }
    cout << "NO COMMON CHARACTER FOUND!" << endl;
    throw;
}

uint32_t get_priority_sum_part1(vector<string> lines)
{
    string first, second;
    uint32_t sum = 0;
    for(uint32_t i = 0; i < lines.size(); i++)
    {
        // split to 2 halves
        uint32_t half_length = (lines.at(i).length() / 2);
        first  = lines.at(i).substr(0, half_length);
        second = lines.at(i).substr(half_length, half_length);

        // get the priority
        uint32_t priority = get_common_letter_priority(first, second);
        sum += priority;
    }
    return sum;
}

uint32_t get_priority_sum_part2(vector<string> lines)
{
    uint32_t sum = 0;
    for(uint32_t i = 0; i < lines.size(); i+=3)
    {
        sum += get_common_letter_priority(lines.at(i), lines.at(i+1), lines.at(i+2));
    }
    return sum;
}

int main()
{
    vector<string> lines = load_file("input.txt");
    //uint32_t sum1 = get_priority_sum_part1(lines);
    //cout << "part 1:" << sum1 << endl;
    uint32_t sum2 = get_priority_sum_part2(lines);
    cout << "part 2:" << sum2 << endl;
}
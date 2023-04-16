
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

uint32_t find_max(string file_path)
{
    fstream fp;
    fp.open(file_path, ios::in);
    uint32_t temp_num = 0, temp_sum = 0,  max_sum = 0;
    string str;
    while(getline(fp, str))
    {
        if(str.length())
        {
            temp_num = stoi(str);
            temp_sum += temp_num;
        }
        else
        {
            if(temp_sum > max_sum)
            {
                max_sum = temp_sum;
            }
            temp_sum = 0;
        }
    }
    return max_sum;
}

void item_swap(vector<uint32_t> &vec, uint32_t first_index, uint32_t second_index)
{
    uint32_t len = vec.size();
    if(first_index >= len || first_index < 0 || second_index >= len || second_index < 0)
    {
        cout << "swap(): input indexes are not in range of vector" << endl;
    }
    uint32_t temp = vec.at(first_index);
    vec.at(first_index) = vec.at(second_index);
    vec.at(second_index) = temp;
}

void max_sort(vector<uint32_t> &vec)
{
    uint32_t temp_max = 0, temp_max_index = -1, len = vec.size();
    for(uint32_t i = 0; i < len; i++)
    {
        // find the max in [0: length - 1 - i] range
        for(uint32_t j = 0; j + i < len; j++)
        {
            if(vec.at(j) > temp_max)
            {
                temp_max = vec.at(j);
                temp_max_index = j;
            }
        }
        if(temp_max_index == -1)
        {
            continue;
        }
        // swap the max with index length - 1 - i
        item_swap(vec, temp_max_index, len - i - 1);
        // cleanup for next iteration
        temp_max = 0;
        temp_max_index = -1;
    }
}

uint32_t find_max_three(string file_path)
{
    fstream fp;
    fp.open(file_path, ios::in);
    vector<uint32_t> sums;
    string str;
    uint32_t temp_num = 0, temp_sum = 0;
    // create the array of sums
    while(getline(fp, str))
    {
        if(str.length())
        {
            temp_num = stoi(str);
            temp_sum += temp_num;
        }
        else
        {
            sums.push_back(temp_sum);
            temp_sum = 0;
        }
    }
    // sort the array
    max_sort(sums);
    // return the sum of the max 3
    temp_sum = 0;
    for(uint32_t i = 0; i < 3; i++)
    {
        temp_sum += sums.at(sums.size() - 1 - i);
    }
    return temp_sum;
}

void sol1()
{
    uint32_t max_sum = find_max("input.txt");
    cout << max_sum << endl;
}

void sol2()
{
    uint32_t max_sum = find_max_three("input.txt");
    cout << max_sum << endl;
}

int main()
{
    sol2();
}
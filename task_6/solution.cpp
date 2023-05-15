#include <iostream>
#include <fstream>
#include <string>

#include "marker_buffer.h"

using namespace std;

int main()
{
    uint32_t buff_size = 14;
    MarkerCyclicBuffer<uint32_t> buffer(buff_size, false);
    
    fstream fp;
    fp.open("input.txt", ios::in);
    char ch;
    uint32_t counter = 0;
    while (fp >> noskipws >> ch) {
        buffer.write(ch);
        counter++;
        if(counter >= buff_size && buffer.is_buffer_unique())
        {
            printf("marker found to be unique after character number %d\n", counter);
            break;
        }
    }
}
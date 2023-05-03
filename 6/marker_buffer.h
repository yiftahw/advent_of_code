#ifndef MARKER_BUFFER_H
#define MARKER_BUFFER_H

#include "cyclic_buffer.h"

template <typename T>
class MarkerCyclicBuffer : public CyclicBuffer<T>
{
public:
    MarkerCyclicBuffer(uint32_t size, bool throw_on_errors = true) : CyclicBuffer<T>(size, throw_on_errors) { }
    bool is_buffer_unique() {
        for(uint32_t i = 0; i < this->m_buffer_size; i++) {
            for(uint32_t j = i + 1; j < this->m_buffer_size; j++) {
                if(this->m_buffer[i] == this->m_buffer[j])
                    return false;
            }
        }
        return true;
    }
};

#endif
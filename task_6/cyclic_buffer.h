#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

#include <iostream>

struct BufferFull : public std::exception
{
	const char * what () const throw ()
    {
    	return "Buffer Is Full!";
    }
};

struct BufferEmpty : public std::exception
{
	const char * what () const throw ()
    {
    	return "Buffer Is Empty!";
    }
};

template <typename T>
class CyclicBuffer
{
protected:
    T* m_buffer = nullptr;
    uint32_t m_buffer_size = 0;
    uint32_t m_data_available = 0;
    uint32_t m_read_idx = 0;
    uint32_t m_write_idx = 0;
    bool m_throw_on_errors = true; 
public:
    CyclicBuffer(uint32_t size, bool throw_on_errors = true) {
        m_buffer = new T[size];
        m_buffer_size = size;
        m_throw_on_errors = throw_on_errors;
    }
    ~CyclicBuffer() {
        if(m_buffer) {
            delete[] m_buffer;
        }
    }
    uint32_t get_size() {
        return m_buffer_size;
    }
    uint32_t get_data_available() {
        return m_data_available;
    }
    void write(T data) {
        if(m_data_available >= m_buffer_size) {
            if(m_throw_on_errors)
                throw BufferFull();
            //else
            //    std::cerr << "Warning: buffer is full!" << std::endl;
        }
        m_buffer[m_write_idx] = data;
        m_write_idx = (m_write_idx + 1) % m_buffer_size;
        if(m_data_available < m_buffer_size)
            m_data_available++;
    }
    T read() {
        if(m_data_available == 0) {
            if(m_throw_on_errors)
                throw BufferEmpty();
            //else
            //    std::cerr << "Warning: buffer is empty, reading zeros!" << std::endl;
        }
        T result = m_buffer[m_read_idx];
        m_buffer[m_read_idx] = 0;
        m_read_idx = (m_read_idx + 1) % m_buffer_size;
        if(m_data_available > 0)
            m_data_available--;
        return result;
    }
};

#endif
//
// Created by Daniel Kowalski on 07/03/2023.
//

#ifndef TESTCPP_BUFFER_HPP
#define TESTCPP_BUFFER_HPP

#include "saving_frames/ReferenceCounter/ReferenceCounter.hpp"

class Buffer {
private:
    unsigned long _size;
    unsigned char* _data;
    ReferenceCounter* _counter;
public:
    unsigned long size() const;
    unsigned char* data();
    Buffer(const unsigned long size);
	Buffer& operator=(const Buffer& buffer);
    Buffer(const Buffer& buffer);
    ~Buffer();
};


#endif //TESTCPP_BUFFER_HPP

//
// Created by Daniel Kowalski on 07/03/2023.
//

#ifndef TESTCPP_BUFFERPOOL_HPP
#define TESTCPP_BUFFERPOOL_HPP

#include "Buffer.hpp"
#include <vector>
#include <map>
#include <mutex>

class BufferPool {
private:
    std::map<int, std::vector<Buffer>*> _index;
    std::mutex _binary_semaphore;

    BufferPool();
    BufferPool(const BufferPool& other);
public:
    Buffer rent(const int size);
    void returnBuffer(const Buffer &buffer);
    static BufferPool Shared;
};


#endif //TESTCPP_BUFFERPOOL_HPP

//
// Created by Daniel Kowalski on 07/03/2023.
//

#include "BufferPool.hpp"

void BufferPool::returnBuffer(const Buffer &buffer) {
    _binary_semaphore.lock();

    std::vector<Buffer>* vector = _index[buffer.size()];
    vector->push_back(buffer);

    _binary_semaphore.unlock();
}

Buffer BufferPool::rent(const int size) {
    _binary_semaphore.lock();

    if (_index.find(size) == _index.end()) {
        _index[size] = new std::vector<Buffer>();
    }

    std::vector<Buffer>* vector = _index[size];

    if (vector->empty()) {
        _binary_semaphore.unlock();
        return *new Buffer(size);
    }

    Buffer buffer = vector->back();
    vector->pop_back();

    _binary_semaphore.unlock();
    return buffer;
}

BufferPool::BufferPool(const BufferPool &other) {
    _index = other._index;
}

BufferPool::BufferPool() {

}

BufferPool BufferPool::Shared;
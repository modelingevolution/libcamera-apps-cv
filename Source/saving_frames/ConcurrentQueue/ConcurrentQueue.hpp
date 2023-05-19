//
// Created by Daniel Kowalski on 07/03/2023.
//

#ifndef TESTCPP_CONCURRENTQUEUE_HPP
#define TESTCPP_CONCURRENTQUEUE_HPP

#include "saving_frames/ReferenceCounter/ReferenceCounter.hpp"
#include "saving_frames/Semaphore/CountingSemaphore.h"
#include <mutex>
#include <queue>

template <typename T>
class ConcurrentQueue  {
private:
    std::queue<T>* _queue;
    CountingSemaphore* _semaphore;
    std::mutex* _binarySemaphore;
    ReferenceCounter* _counter;
public:
    T pop();
    void push(const T &element);
    ConcurrentQueue();
    ~ConcurrentQueue();
    ConcurrentQueue(const ConcurrentQueue& other);
	ulong size();
};

template <typename T>
ulong ConcurrentQueue<T>::size()
{
	return _queue->size();
}

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue(const ConcurrentQueue &other) {
    this->_queue = other._queue;
    this->_semaphore = other._semaphore;
    this->_binarySemaphore = other._binarySemaphore;
    this->_counter = other._counter;
    _counter->increase();
}

template<typename T>
ConcurrentQueue<T>::~ConcurrentQueue() {
    if (_counter->decrease()) {
        delete _queue;
        delete _semaphore;
        delete _binarySemaphore;
        delete _counter;
    }
}

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue() {
    _queue = new std::queue<T>();
	_binarySemaphore = new std::mutex();
    _semaphore = new CountingSemaphore(0);
    _counter = new ReferenceCounter();
}

template<typename T>
void ConcurrentQueue<T>::push(const T &element) {
    this->_binarySemaphore->lock();
    _queue->push(element);
    this->_binarySemaphore->unlock();
    this->_semaphore->release();
}

template<typename T>
T ConcurrentQueue<T>::pop() {
    this->_semaphore->acquire();
    this->_binarySemaphore->lock();
    auto tmp = _queue->front();
    _queue->pop();
    this->_binarySemaphore->unlock();
    return tmp;
}

#endif //TESTCPP_CONCURRENTQUEUE_HPP

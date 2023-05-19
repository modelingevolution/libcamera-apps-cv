//
// Created by Daniel Kowalski on 08/03/2023.
//

#include "ReferenceCounter.hpp"

bool ReferenceCounter::decrease() {
    return _counter.fetch_add(-1) == 1;
}

void ReferenceCounter::increase() {
    _counter.fetch_add(1);
}

ReferenceCounter::ReferenceCounter(const ReferenceCounter &) {
    throw std::runtime_error("Not permitted.");
}

ReferenceCounter::ReferenceCounter() {
    _counter.store(0, std::memory_order_release);
}

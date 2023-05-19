//
// Created by Daniel Kowalski on 08/03/2023.
//

#ifndef TESTCPP_REFERENCECOUNTER_HPP
#define TESTCPP_REFERENCECOUNTER_HPP

#include <atomic>
#include <stdexcept>

class ReferenceCounter {
private:
    std::atomic<int> _counter;

public:
    ReferenceCounter();

    ReferenceCounter(const ReferenceCounter&);

    void increase();

    bool decrease();
};


#endif //TESTCPP_REFERENCECOUNTER_HPP

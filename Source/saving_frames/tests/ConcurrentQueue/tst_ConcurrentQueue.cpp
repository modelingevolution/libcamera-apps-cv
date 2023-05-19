//
// Created by Daniel Kowalski on 07/03/2023.
//

#include "gtest/gtest.h"
#include "ConcurrentQueue.hpp"
#include <thread>

TEST(ConcurrentQueue, test_push_and_pop) {
    ConcurrentQueue<int> queue;

    queue.push(2);
    queue.push(3);
    queue.push(4);

    int x = queue.pop();
    int y = queue.pop();
    int z = queue.pop();

    EXPECT_EQ(x, 2);
    EXPECT_EQ(y, 3);
    EXPECT_EQ(z, 4);
}

void task(ConcurrentQueue<int>* queue, int* x) {
    sleep(2);

    if (*x == 0) {
        queue->push(2);
        sleep(2);
    }
}

TEST(ConcurrentQueue, pop_should_wait_if_there_is_nothing_in_queue) {
    ConcurrentQueue<int> queue;
    int x = 0;

    std::thread t(task, &queue, &x);

    x = queue.pop();

    t.join();

    EXPECT_EQ(x, 2);
}

TEST(ConscurrentQueue, copy_constructor) {
    ConcurrentQueue<int> queue;

    queue.push(1);

    ConcurrentQueue<int> anotherQueue = queue;

    int x = anotherQueue.pop();

    EXPECT_EQ(x, 1);
}
//
// Created by Daniel Kowalski on 07/03/2023.
//

#include "gtest/gtest.h"
#include "BufferPool.hpp"

TEST(BufferPool, rent_and_return) {
    Buffer buffer = BufferPool::Shared.rent(20);

    EXPECT_EQ(buffer.size(), 20);

    BufferPool::Shared.returnBuffer(buffer);

    Buffer another = BufferPool::Shared.rent(20);

    EXPECT_EQ(another.data(), buffer.data());
}
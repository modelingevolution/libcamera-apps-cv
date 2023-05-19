//
// Created by Daniel Kowalski on 08/03/2023.
//

#include "gtest/gtest.h"
#include "ConcurrentQueue.hpp"
#include "BufferPool.hpp"
#include <thread>

FILE*framesFile;
const int size = 100;
const char* filename = "frames.bin";
ConcurrentQueue<Buffer> recognitionQueue;
ConcurrentQueue<Buffer> saveQueue;
const int iterations = 10;

void copy() {
    uint8_t data[size];

    for (int i = 0; i < iterations; i++) {
        Buffer buffer = BufferPool::Shared.rent(size);
        std::fill_n(data, size, i);
        memcpy(buffer.data(), data, size);
        recognitionQueue.push(buffer);
        sleep(1);
    }
}

void recognize() {
    int i = 0;
	const int h = 5;
	const int w = 20;
	uint8_t mat_line[w];

    while (i < iterations) {
        Buffer buffer = recognitionQueue.pop();

		uint8_t *ptr = buffer.data();
		for (int j = 0; j < h; j++, ptr += w)
		{
			memcpy(mat_line, ptr, w);
		}

        saveQueue.push(buffer);
        i++;
    }
}

void save() {
    long i = 1;

    while (i <= iterations) {
        Buffer buffer = saveQueue.pop();

        std::fwrite(&i, sizeof(long), 1, framesFile);
        std::fwrite(buffer.data(), sizeof(uint8_t), size, framesFile);

        BufferPool::Shared.returnBuffer(buffer);
        i++;
    }
}

TEST(Integration, copy_recognition_and_saving_works_together) {
	framesFile = std::fopen(filename, "wb");

    std::thread copyThread = std::thread([] { copy(); });
    std::thread recognizeThread = std::thread([] { recognize(); });
    std::thread saveThread = std::thread([] { save(); });

    copyThread.join();
    recognizeThread.join();
    saveThread.join();

    std::fclose(framesFile);

	framesFile = std::fopen(filename, "rb");

    long index;
    uint8_t frame[size];

    for (int i = 1; i <= iterations; i++) {
        std::fread(&index, sizeof(long), 1, framesFile);
        std::fread(&frame, sizeof(uint8_t), size, framesFile);

        EXPECT_EQ(index, i);

        for (int j = 0; j < size; j++) {
            unsigned char x = frame[j];
            EXPECT_EQ(x, i - 1);
        }
    }

    std::fclose(framesFile);

}
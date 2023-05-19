//
// Created by Daniel Kowalski on 08/03/2023.
//

#ifndef LIBCAMERA_APPS_COUNTINGSEMAPHORE_H
#define LIBCAMERA_APPS_COUNTINGSEMAPHORE_H

#include <mutex>
#include <condition_variable>

class CountingSemaphore
{
private:
	std::mutex _mutex;
	std::condition_variable _condition;
	unsigned long _count;

public:
	CountingSemaphore(unsigned long count);
	void release();
	void acquire();
};

#endif //LIBCAMERA_APPS_COUNTINGSEMAPHORE_H

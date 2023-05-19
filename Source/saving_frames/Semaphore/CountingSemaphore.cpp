//
// Created by Daniel Kowalski on 08/03/2023.
//

#include "CountingSemaphore.h"
CountingSemaphore::CountingSemaphore(unsigned long count)
{
	_count = count;
}
void CountingSemaphore::release()
{
	std::lock_guard<decltype(_mutex)> lock(_mutex);
	++_count;
	_condition.notify_one();
}
void CountingSemaphore::acquire()
{
	std::unique_lock<decltype(_mutex)> lock(_mutex);
	while(!_count) // Handle spurious wake-ups.
		_condition.wait(lock);
	--_count;
}

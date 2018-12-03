#include "threadsafequeue.hpp"

template<typename T>
void ThreadSafeQueue<T>::push(const T & value)
{
	std::unique_lock<std::mutex> lock(the_mutex);
	real_queue.push(value);
	lock.unlock();
	the_cond_var.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const
{
	std::lock_guard<std::mutex> lock(the_mutex);
	return real_queue.empty();
}

template<typename T>
bool ThreadSafeQueue<T>::try_pop(T & popped_value)
{
	std::lock_guard<std::mutex> lock(the_mutex);
	if (real_queue.empty())
	{
		return false;
	}

	popped_value = real_queue.front();
	real_queue.pop();
	return true;
}

template<typename T>
void ThreadSafeQueue<T>::wait_and_pop(T & popped_value)
{
	std::unique_lock<std::mutex> lock(the_mutex);
	while (real_queue.empty())
	{
		if(EXIT)
		{
			break;
		}
		the_cond_var.wait(lock);
	}

	popped_value = real_queue.front();
	real_queue.pop();
}

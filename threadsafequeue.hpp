
#ifndef THREADSAFEQUEUE_HPP
#define THREADSAFEQUEUE_HPP

#include <mutex>
#include <queue>
#include <thread>

template<typename T>
class ThreadSafeQueue
{
public:
	void push(const T & value);

	bool empty() const;

	bool try_pop(T & popped_value);

	void wait_and_pop(T & popped_value);
	bool EXIT = false;
private:
	std::queue<T> real_queue;
	mutable std::mutex the_mutex;
	std::condition_variable the_cond_var;
};

#include "threadsafequeue.tpp"
#endif

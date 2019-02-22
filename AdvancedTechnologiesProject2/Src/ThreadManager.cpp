#include "ThreadManager.h"

#include <iostream>

void Worker::operator()()
{
	std::function<void()> task;
	while (true)
	{
		{   // acquire lock
			std::unique_lock<std::mutex>
				lock(pool.queue_mutex);

			// look for a work item
			while (!pool.stop && pool.tasks.empty())
			{ // if there are none wait for notification
				pool.condition.wait(lock);
			}

			if (pool.stop) // exit if the pool is stopped
				return;

			// get the task from the queue
			task = pool.tasks.front();
			pool.tasks.pop_front();

		}   // release lock

		// execute the task
		task();
	}
}

ThreadPool::ThreadPool()
	: stop(false)
{
	for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		workers.push_back(std::thread(Worker(*this)));
	}
}

ThreadPool::~ThreadPool()
{
	stop = true;
	condition.notify_all();

	for (auto& worker : workers)
	{
		worker.join();
	}

	std::cout << taskDone << std::endl;
}

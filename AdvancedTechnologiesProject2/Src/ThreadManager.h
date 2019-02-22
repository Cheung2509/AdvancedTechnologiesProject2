#pragma once

#include <thread>
#include <atomic>
#include <condition_variable>
#include <vector>
#include <deque>

class ThreadPool;

class Worker
{
public:
	Worker(ThreadPool& s) : pool(s) {}
	void operator()();
private:
	ThreadPool& pool;
};

// the actual thread pool
class ThreadPool
{
public:
	ThreadPool();
	template<class F>
	void enqueue(F f);
	~ThreadPool();

	const int& getNumTasks() const { return tasks.size(); }
private:
	friend class Worker;

	// need to keep track of threads so we can join them
	std::vector< std::thread > workers;

	// the task queue
	std::deque< std::function<void()> > tasks;
	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;

	std::atomic<int> taskDone = 0;
};

template<class F>
inline void ThreadPool::enqueue(F f)
{
	{
		std::unique_lock<std::mutex> loc(queue_mutex);

		tasks.push_back(std::function<void()>(f));
	}

	condition.notify_one();
	taskDone++;
}

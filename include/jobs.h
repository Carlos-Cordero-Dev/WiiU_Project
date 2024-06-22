#ifndef JOBS_H_
#define JOBS_H_ 1

#include <queue>
#include <functional>
#include <future>
#include <stdio.h>
#include <memory>

class JobsManager
{

public:

	JobsManager();
	~JobsManager();
	template <typename Function, typename... Args>
	auto enqueue(Function&& task, Args&&... args) -> std::future<decltype(task(std::forward<Args>(args)...))>
	{
		using Ret = decltype(task(std::forward<Args>(args)...));
		auto bound_task = std::bind(std::forward<Function>(task), std::forward<Args>(args)...);
		auto t = std::make_shared<std::packaged_task<Ret()>>(bound_task);
		std::future<Ret> future = t->get_future();

		std::lock_guard<std::mutex> mutexLock{ queue_mutex_ };
		jobs_.push([t]() { (*t)(); });

		queue_condition_.notify_one();

		return future;
	}
private:
	JobsManager& operator=(const JobsManager&) = delete;
	JobsManager(const JobsManager&) = delete;

private:
	bool stopThreads = false;
	std::vector<std::thread> workers_;
	std::queue<std::function<void()>> jobs_;
	std::mutex queue_mutex_;
	std::condition_variable queue_condition_;
};

static void FuncionGorrina(int gorrinada)
{
	//complejidad:
	/*
	Sum[x*x,{x,1,gorrinada}]
	*/
	auto num_of_divs = 0;
	printf("\nEmpieza funcion gorrina");
	std::vector<int> list;

	for (int i = 1; i < gorrinada + 1; i++)
	{
		list.push_back(i);
		for (unsigned int j = 0; j < list.size(); j++)
		{
			for (unsigned int k = 0; k < list.size(); k++)
			{

				//int div = list[j] / list[k];
				num_of_divs++;
			}
		}
	}
	printf("\ntermina (%d divisiones)\n", num_of_divs);
}

#endif // ! __JOBS_H__

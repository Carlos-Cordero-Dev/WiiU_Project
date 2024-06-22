
#include "jobs.h"

JobsManager::JobsManager()
{
	//printf("\nJobsManager created\n");
	auto workers_count = std::thread::hardware_concurrency();
	for (size_t i = 0; i != workers_count; i++)
	{
		/*worker es el tipo de this, que es la funcion. Worker es una funcion*/
		auto worker = [this/*esto es un parametro, el this de toda la vida de una clase, dentro de las lambdas hay que pasarlo
						   por parametro explicitamente, en el resto de casos es implicito */]() {
			while (true)
			{
				//ahora los brackets son para crear scopes para que el lock y el mutex no mueran antes de cumplir su papel
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> mutexLock{ queue_mutex_ };
					queue_condition_.wait(mutexLock, [this] {return stopThreads || !jobs_.empty(); }/*this es un caso especial para poder acceder a variables de */);
					if (jobs_.empty() && stopThreads)
					{
						return;
					}

					job = jobs_.front();
					jobs_.pop();
					printf("\nNumber of jobs %d", (int)jobs_.size());

				}
				if (job) job();
			}
		};
		workers_.push_back(std::thread{ std::move(worker) });
	}
}

JobsManager::~JobsManager()
{
	{
		std::lock_guard<std::mutex> mutexLock{ queue_mutex_ };
		stopThreads = true;
	}

	queue_condition_.notify_all();

	for (unsigned int i = 0; i < workers_.size(); i++) {
		if (workers_[i].joinable())
			workers_[i].join();
	}

	//printf("\n%d workers, jobs manager destroyed\n", (int)workers_.size());
}


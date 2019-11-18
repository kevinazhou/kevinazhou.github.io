#include "stdafx.h"
#include "JobManager.h"
#include "Job.h"
#include "Worker.h"

bool JobManager::shutdown = false;
std::mutex JobManager::mMutex;
std::queue<Job*> JobManager::mQueue;
std::atomic<int> JobManager::jobCount = 0;

JobManager::JobManager()
{
	for (int n = 0; n < 4; n++)
	{
		Worker* toAdd = new Worker();
		workers.push_back(toAdd);
	}
}

JobManager::~JobManager()
{
	for (int n = 0; n < workers.size(); n++)
	{
		delete workers[n];
	}
}

void JobManager::Begin()
{
	for (int j = 0; j < workers.size(); j++)
	{
		workers[j]->Begin();
	}
}

void JobManager::End()
{
	for (int b = 0; b < workers.size(); b++)
	{
		workers[b]->End();
	}
}

void JobManager::AddJob(Job* pJob)
{
	mMutex.lock();
	jobCount++;
	mQueue.push(pJob);
	mMutex.unlock();
}

void JobManager::WaitForJobs()
{
	while (jobCount > 0)
	{
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}
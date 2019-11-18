#pragma once
#include "stdafx.h"
#include "Job.h"
#include "Worker.h"
#include "JobManager.h"

Worker::Worker()
{
}

Worker::~Worker()
{
	delete mThread;
}

void Worker::Begin()
{
	mThread = new std::thread(Worker::Loop);
}

void Worker::End()
{
	mThread->join();
	//delete mThread; code never reaches here
}

void Worker::Loop()
{
	while (true)
	{
		if (JobManager::GetStatus())
		{
			return;
		}
		JobManager::mMutex.lock();
		if (!JobManager::mQueue.empty())
		{
			Job* newJob = JobManager::mQueue.front();
			JobManager::mQueue.pop();
			JobManager::mMutex.unlock();
			newJob->DoIt();
			delete newJob;
			JobManager::jobCount--;
		}
		else
		{
			JobManager::mMutex.unlock();
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
		
	}
}
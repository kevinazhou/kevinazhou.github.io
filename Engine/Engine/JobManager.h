#pragma once
#include <mutex>
#include <queue>
#include <atomic>


class JobManager
{
public:
	JobManager();
	~JobManager();
	void Begin();
	void End();
	void AddJob(class Job* pJob);
	void WaitForJobs();
	static bool GetStatus() { return shutdown; };
	static std::queue<class Job*> mQueue;
	static std::mutex mMutex;
	static std::atomic<int> jobCount;
private:
	static bool shutdown;
	std::vector<class Worker*> workers;
};
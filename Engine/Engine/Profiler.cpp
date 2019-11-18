#include "stdafx.h"
#include "Profiler.h"

Profiler Profiler::profiler;
FILE* Profiler::TraceFile;

void Profiler::Timer::Start()
{
	mStartTime = std::chrono::high_resolution_clock::now();

	std::string TraceStart = "{\"name\": \" ";
	TraceStart += GetName();
	TraceStart += "\", \"cat\": \"PERF\" , \"ph\":\"B\", \"ts\":";
	TraceStart += std::to_string(mStartTime.time_since_epoch().count() / 1000);
	TraceStart += " , \"pid\":1, \"tid\":1 }, ";
	Get()->TraceEntry += TraceStart;
}

void Profiler::Timer::Stop()
{
	std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	double diff = (double)std::chrono::duration_cast<std::chrono::nanoseconds> (endTime - mStartTime).count();
	diff *= (1.0 / 1000000.0);
	mCurrentFrameMS += diff;

	std::string TraceStop = "{\"ph\":\"E\", \"ts\":";
	TraceStop += std::to_string(endTime.time_since_epoch().count() / 1000);
	TraceStop += " , \"pid\":1, \"tid\":1},";
	Get()->TraceEntry += TraceStop;
}

void Profiler::Timer::Reset()
{
	if (mCurrentFrameMS > 0.0)
	{
		mTotalFrameTimeMS += mCurrentFrameMS;
		mFramesCaptured++;
		if (mLongestFrameMS < mCurrentFrameMS)
		{
			mLongestFrameMS = mCurrentFrameMS;
		}
		mCurrentFrameMS = 0;
	}
}

const std::string& Profiler::Timer::GetName() const
{
	return mTimerName;
}

double Profiler::Timer::GetTime_ms() const
{
	return mCurrentFrameMS;
}

double Profiler::Timer::GetMax_ms() const
{
	return mLongestFrameMS;
}

double Profiler::Timer::GetAvg_ms() const
{
	return (double)(mTotalFrameTimeMS / mFramesCaptured);
}

Profiler::Timer* Profiler::GetTimer(const std::string& name)
{
	auto iter = mTimers.find(name);
	if (iter != mTimers.end())
	{//timer found
		return mTimers[name];
	}
	else
	{
		Timer* newTimer = new Profiler::Timer();
		mTimers.insert(std::make_pair(name, newTimer));
		mTimers[name]->mTimerName = name;
		return mTimers[name];
	}
}

Profiler::ScopedTimer::ScopedTimer(Timer* timer)
	:mTimer(timer)
{
	mTimer->Start();
}

Profiler::ScopedTimer::~ScopedTimer()
{
	mTimer->Stop();
}

Profiler::Profiler()
{
	std::string TraceFilename = "profile.json";
	std::string mode = "w";
	fopen_s(&TraceFile, TraceFilename.c_str(), mode.c_str());
	TraceEntry += "[";
}

Profiler::~Profiler()
{
	std::string filename = "profile.txt";
	std::string mode = "w";
	FILE* file;
	fopen_s(&file, filename.c_str(), mode.c_str());
	std::string header = "name:, avg(ms), max(ms)\n";
	fprintf(file, header.c_str());

	for (auto it = mTimers.begin(); it != mTimers.end(); it++)
	{
		std::string str;
		str += it->second->GetName();
		str += ":, ";
		str += std::to_string(it->second->GetAvg_ms());
		str += ", ";
		str += std::to_string(it->second->GetMax_ms());
		str += "\n";
		fprintf(file, str.c_str());
	}
	fclose(file);


	if (TraceEntry.size() > 0)
	{
		TraceEntry.pop_back();
	}
	TraceEntry += "]";
	fprintf(TraceFile, TraceEntry.c_str());
	fclose(TraceFile);
}

void Profiler::ResetAll()
{
	for (auto iter = mTimers.begin(); iter != mTimers.end(); iter++)
	{
		iter->second->Reset();
	}
}
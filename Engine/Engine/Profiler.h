#pragma once
#include <string>
#include <chrono>
#include <unordered_map>

class Profiler
{
public:
	class Timer
	{
		friend Profiler;
	public:
		void Start();
		void Stop();
		void Reset();
		const std::string& GetName() const;
		double GetTime_ms() const;
		double GetMax_ms() const;
		double GetAvg_ms() const;
	private:
		Timer() {};
		~Timer() {};

		std::string mTimerName;
		
		double mCurrentFrameMS = 0;

		double mLongestFrameMS = 0;

		double mTotalFrameTimeMS = 0;

		int mFramesCaptured = 0;

		std::chrono::high_resolution_clock::time_point mStartTime;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(Timer* timer);
		~ScopedTimer();
	private:
		Timer* mTimer;
	};

	friend Timer;
	static Profiler* Get() { return &profiler; };
	Timer* GetTimer(const std::string& name);
	void ResetAll();
private:
	static Profiler profiler;
	static FILE* TraceFile;
	std::string TraceEntry = "";
	Profiler();
	~Profiler();
	std::unordered_map<std::string, Timer*> mTimers;
};

#define PROFILE_SCOPE(name) \
Profiler::ScopedTimer name##_scope(Profiler::Get()->GetTimer(std::string(#name)))
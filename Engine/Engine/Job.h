#pragma once

class Job
{
public:
	Job() {};
	~Job() {};
	virtual void DoIt() = 0;
};
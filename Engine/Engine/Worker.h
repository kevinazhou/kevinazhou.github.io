#include <thread>

class Worker
{
public:
	Worker();
	~Worker();
	void Begin();
	void End();
	static void Loop();

private:
	std::thread* mThread;
};
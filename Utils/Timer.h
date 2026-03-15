#ifndef _TIMER_H
#define _TIMER_H


//#include <sstream>   // wstringstream
#include <iomanip>
#include <chrono>
#include <ctime>
class Timer
{
	bool stillRunning;
	std::chrono::steady_clock::time_point stopWatchStartPoint;
	std::chrono::steady_clock::time_point stopWatchEndPoint;
public:
	Timer(): stillRunning(false)
	{}
	~Timer() = default;

	void Start();
	void End();
	double GetDuration();

	std::tm GetCurrTime();

	std::chrono::steady_clock::time_point GetTimeMs();
};

#endif


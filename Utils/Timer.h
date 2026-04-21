#ifndef _TIMER_H
#define _TIMER_H


//#include <sstream>   // wstringstream
#include <iomanip>
#include <chrono>
#include <ctime>

using TimePoint = std::chrono::steady_clock::time_point;
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
	
	double GetDuration(const TimePoint start, const TimePoint end);
	double GetDuration();

	std::tm GetCurrTime();
	
	TimePoint GetCurrTimeMs();
};

#endif


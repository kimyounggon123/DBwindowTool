#include "Timer.h"
void Timer::Start()
{
	stopWatchStartPoint = std::chrono::high_resolution_clock::now();
	stillRunning = true;
}
void Timer::End()
{ 
	stopWatchEndPoint = std::chrono::high_resolution_clock::now();
	stillRunning = false;
}


double Timer::GetDuration(const TimePoint start, const TimePoint end)
{
	std::chrono::duration<double, std::milli> diff = end - start;
	return diff.count();
}
double Timer::GetDuration()
{
	return GetDuration(stopWatchStartPoint, stopWatchStartPoint);
}

std::tm Timer::GetCurrTime()
{
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);

	std::tm tm{};
	localtime_s(&tm, &t);   // Windows 안전 버전
	return tm;
}

TimePoint Timer::GetCurrTimeMs()
{
	return std::chrono::high_resolution_clock::now();
}
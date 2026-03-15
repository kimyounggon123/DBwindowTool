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
double Timer::GetDuration()
{
	std::chrono::duration<double, std::milli> diff = stopWatchEndPoint - stopWatchStartPoint;
	return diff.count();
}

std::tm Timer::GetCurrTime()
{
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);

	std::tm tm{};
	localtime_s(&tm, &t);   // Windows 안전 버전
	return tm;
}

std::chrono::steady_clock::time_point Timer::GetTimeMs()
{
	return std::chrono::high_resolution_clock::now();
}
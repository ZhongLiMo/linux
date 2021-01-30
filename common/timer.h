#ifndef _TIMER_H_
#define _TIMER_H_

#include <set>
#include <functional>

class Timer;
struct TimerCompare
{
	bool operator()(const Timer* left, const Timer* right) const;
};
using TimerSet = std::set<Timer*, TimerCompare>;
class TimerManager
{
public:
	TimerManager();
	virtual ~TimerManager();
public:
	void OnTimer();
    void InitTimer(unsigned int timer_stamp) { timer_stamp_ms = timer_stamp; }
private:
	void RemoveTimer(Timer* timer);
	void RegisterTimer(Timer* timer);
private:
	time_t						next_time;
	bool						change_iter;
	TimerSet					listeners;
	TimerSet::iterator			timer_iter;
    unsigned int                timer_stamp_ms;
	friend class Timer;
};

class Timer
{
public:
	template<typename callback, typename... arguments>
	Timer(callback&& fun, arguments&&... args) :
		task(std::bind(std::forward<callback>(fun), std::forward<arguments>(args)...)),
		next_time(0), loop_time(0) {}
	virtual ~Timer();
public:
	void StopTimer();
	void StartTimer(int delay_time, bool loop = false);
	void RestartTimer(int delay_time, bool loop = false);
private:
	int						loop_time;
	time_t					next_time;
	std::function<void()>	task;
	friend class TimerManager;
	friend struct TimerCompare;
};

#endif // !_TIMER_H_
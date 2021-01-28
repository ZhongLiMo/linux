#include "timer.h"
#include "basefunc.h"

static const int DEAFULT_TIMER_STAMP_MS = 10;

extern TimerManager timerManager;

TimerManager::TimerManager() : next_time(0), change_iter(true), listeners(), timer_iter(listeners.end()), timer_stamp_ms(DEAFULT_TIMER_STAMP_MS)
{
}
TimerManager::~TimerManager()
{
}
void TimerManager::OnTimer()
{
	if (listeners.empty()) return;
	time_t cur_time = BASE_FUNC::GetCurTimeMS();
	if (next_time < cur_time)
	{
		next_time = cur_time + timer_stamp_ms;
		for (timer_iter = listeners.begin(); timer_iter != listeners.end(); )
		{
			if ((*timer_iter)->next_time <= cur_time)
			{
				(*timer_iter)->task();
				if ((*timer_iter)->loop_time)
				{
					(*timer_iter)->next_time = cur_time + (*timer_iter)->loop_time;
				}
				else
				{
					RemoveTimer(*timer_iter);
				}
			}
			else
			{
				change_iter = true;
				timer_iter = listeners.end();
				break;
			}
			if (change_iter)
			{
				++timer_iter;
			}
			else
			{
				change_iter = true;
			}
		}
	}
}
void TimerManager::RemoveTimer(Timer* timer)
{
	if (!timer) return;
	TimerSet::iterator ite = listeners.find(timer);
	if (ite != listeners.end())
	{
		if (ite == timer_iter)
		{
			change_iter = false;
			timer_iter = listeners.erase(ite);
		}
		else
		{
			listeners.erase(ite);
		}
	}
}
void TimerManager::RegisterTimer(Timer* timer)
{
	if (!timer) return;
	if (listeners.find(timer) == listeners.end())
	{
		bool change = timer_iter == listeners.end();
		listeners.insert(timer);
		if (change)
			timer_iter = listeners.end();
	}
}

Timer::~Timer()
{
	timerManager.RemoveTimer(this);
}
void Timer::StopTimer()
{
	timerManager.RemoveTimer(this);
}
void Timer::StartTimer(int delay_time, bool loop)
{
	timerManager.RemoveTimer(this);
	if (delay_time < 0) return;
	loop_time = loop ? delay_time : 0;
	next_time = BASE_FUNC::GetCurTimeMS() + delay_time;
	timerManager.RegisterTimer(this);
}
void Timer::RestartTimer(int delay_time, bool loop)
{
	StartTimer(delay_time, loop);
}
bool TimerCompare::operator()(const Timer* left, const Timer* right) const
{
	return left->next_time < right->next_time;
}
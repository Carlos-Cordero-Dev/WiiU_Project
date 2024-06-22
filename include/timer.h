
#ifndef TIMER_H_
#define TIMER_H_ 1
 
#include <coreinit/time.h>

int deltaTime_;
int gameTime_;

int gameStart_;
int gameFrameBegin_;
int gameFrameEnd_;
int gameFrameMark_;

int inline getTicksInMS()
{
	return OSTicksToMilliseconds(OSGetTick());
}

//TODO Tener en cuenta la pausa
double GetGameTime()
{
	return (double)gameTime_;
}

double GetDeltaTime()
{
	return (double)deltaTime_;
}

void TimerReset()
{
	gameStart_ = getTicksInMS();
	gameFrameBegin_ = gameStart_;
}

void TimerUpdate()
{
	int currentTime = getTicksInMS();
	gameTime_ = currentTime - gameStart_;
	deltaTime_ = currentTime - gameFrameBegin_;
	gameFrameBegin_ = currentTime;
}

#endif // !TIMER_H_



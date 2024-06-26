
#ifndef TIMER_H_
#define TIMER_H_ 1
 
#include <coreinit/time.h>
#include <chrono>

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

//=== LOVE INPLEMENTATION =====

//https://github.com/lovebrew/lovepotion/blob/dev/graphics/platform/cafe/source/modules/timer/Timer.cpp

static OSTick reference = 0;
static constexpr double SECONDS_TO_NS = 1.0E9;
static int frames = 0;
static double prevTime = 0.0;
static double currTime = 0.0;
static double deltaTime = 0.0;
static double prevFpsUpdate = 0.0;
static double fpsUpdateFrequency = 1.0;
static double averageDelta = 0.0;
static int fps = 0;

double getLOVETime()
{
    const auto ns = OSTicksToNanoseconds(OSGetTick() - reference);
    return ns / SECONDS_TO_NS;
}

StartLOVETimer()
{
    reference = OSGetTick();
    currTime = getLOVETime();
    prevFpsUpdate = currTime;
}

void LOVETimersleep(double seconds)
{
    const auto time = std::chrono::duration<double>(seconds);
    const auto nanoseconds = std::chrono::duration<double, std::nano>(time).count();

    OSSleepTicks(OSNanosecondsToTicks(nanoseconds));
}

double LOVETimerStep()
{
    frames++;

    prevTime = currTime;
    currTime = getLOVETime();

    deltaTime = currTime - prevTime;

    double timeSinceLast = (currTime - prevFpsUpdate);

    if (timeSinceLast > fpsUpdateFrequency)
    {
        fps = int((frames / timeSinceLast) + 0.5);
        averageDelta = timeSinceLast / frames;
        prevFpsUpdate = currTime;
        frames = 0;
    }

    return deltaTime;
}

#endif // !TIMER_H_



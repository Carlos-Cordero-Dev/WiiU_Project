
#ifndef TIMER_H_
#define TIMER_H_ 1
 
#include <chrono>
#include <thread>

//https://github.com/lovebrew/lovepotion/blob/dev/graphics/platform/cafe/source/modules/timer/Timer.cpp adapted to stdlib

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<double>;
using Nanoseconds = std::chrono::duration<double, std::nano>;
constexpr double SECONDS_TO_NS = 1.0E9;

class Timer
{
public:
    Timer();

    double get_time();
    void sleep(double seconds);
    //returns deltatime in sec
    double step();

public:
    TimePoint reference;
    int frames = 0;
    double prevTime = 0.0;
    double currTime = 0.0;
    double deltaTime = 0.0;
    double prevFpsUpdate = 0.0;
    double fpsUpdateFrequencyInSec = 1.0;
    double averageDelta = 0.0;
    int fps = 0;
};

Timer::Timer()
{
    reference = Clock::now();
    currTime = get_time();
    prevFpsUpdate = currTime;
}

double Timer::get_time()
{
    auto now = Clock::now();
    auto ns = std::chrono::duration_cast<Nanoseconds>(now - reference).count();
    return ns / SECONDS_TO_NS;
}


void Timer::sleep(double seconds)
{
    auto duration = std::chrono::duration<double>(seconds);
    std::this_thread::sleep_for(duration);
}

double Timer::step()
{
    frames++;

    prevTime = currTime;
    currTime = get_time();

    deltaTime = currTime - prevTime;

    double timeSinceLast = (currTime - prevFpsUpdate);

    if (timeSinceLast > fpsUpdateFrequencyInSec)
    {
        fps = int((frames / timeSinceLast) + 0.5);
        averageDelta = timeSinceLast / frames;
        prevFpsUpdate = currTime;
        frames = 0;
    }

    return deltaTime;
}

#endif // !TIMER_H_



#ifndef FPSCOUNTER__
#define FPSCOUNTER__
#include <SFML/System/Clock.hpp>

class FPSCounter
{
private:
	sf::Clock clock;
	size_t frames;
	size_t fps;
public:
	FPSCounter()
	: frames(0)
	, fps(0)
	{
	}
	size_t Update()
	{
		frames++;
		int32_t timeElapsed = clock.getElapsedTime().asMilliseconds();
		if(timeElapsed > 2000)
		{
			fps = frames * 1000 / (float)timeElapsed;
			clock.restart();
			frames = 0;
		}
		return fps;
	}
};

#endif


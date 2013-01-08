#ifndef TIMESTEPPER__
#define TIMESTEPPER__
#include <SFML/System/Clock.hpp>

class TimeStepper
{
private:
	sf::Clock clock;
	int32_t begintime;
	size_t stepssimulated;
	size_t fps;
public:
	TimeStepper(size_t fps)
	: begintime(clock.getElapsedTime().asMilliseconds())
	, stepssimulated(0)
	, fps(fps)
	{
	}
	size_t steps()
	{
		size_t ellapsedtime = clock.getElapsedTime().asMilliseconds() - begintime;
		size_t stepssimulatedafter = ellapsedtime * (fps / (float)1000);
		size_t steps = stepssimulatedafter - stepssimulated;
		stepssimulated = stepssimulatedafter;
		return steps;
	}
};

#endif

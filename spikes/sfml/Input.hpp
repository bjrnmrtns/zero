#ifndef INPUT__
#define INPUT__

class Input
{
public:
	virtual void Update(float timepassed) = 0;
};

class NoInput : public Input
{
public:
	void Update(float timepassed)
	{
		timepassed = timepassed;
	}
};

#endif

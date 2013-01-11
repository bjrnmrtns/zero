#ifndef INPUT__
#define INPUT__

class Input
{
public:
	virtual void Update(size_t nrofsteps, float stepsize) = 0;
};

class NoInput : public Input
{
public:
	void Update(size_t nrofsteps, float stepsize)
	{
	}
};

#endif

#ifndef INPUT__
#define INPUT__

class Input
{
public:
	virtual void Update() = 0;
};

class NoInput : public Input
{
public:
	void Update()
	{
	}
};

#endif

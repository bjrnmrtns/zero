#include <typeinfo>
#include <iostream>
#include <vector>

static size_t indent = 0;

class Inspection
{
public:
	virtual void Inspect() const = 0;
};

void AddChildInspection(const Inspection &inspection)
{
	inspection.Inspect();
}

template <typename T>
void AddInspection(const T &variable)
{
	std::cout << typeid(T).name() << ": " << variable << std::endl;
}

class Box : public Inspection
{
	int y;
	float g;
	void Inspect() const
	{
		AddInspection(y);
		AddInspection(g);
	}
};

class Entity : public Inspection
{
public:
	int x;
	float r;
	Box b;
	void Inspect() const
	{
		AddInspection(x);
		AddInspection(r);
		AddChildInspection(b);
	}
};

int main()
{
	Entity entity;
	entity.Inspect();
	return 0;
}


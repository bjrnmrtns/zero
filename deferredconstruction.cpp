#include <string>
#include <iostream>
#include <memory>

class A
{
public:
class Factory
{
public:
	int a;
	char b;
	Factory(int a, char b)
	: a(a)
	, b(b)
	{
	}
	A* Create()
	{
		return new A(a, b);
	}
};

	int a;
	char b;
	A(int a, char b)
	: a(a)
	, b(b)
	{
	}
};

template <typename T, typename Factory>
static T* LoadResource(std::string name, Factory f)
{
	std::cout << "Load for name: " << name << std::endl;
	return f.Create();
}

int main()
{
	std::unique_ptr<A> a(LoadResource<A>("color", A::Factory(3, 'x')));
	std::cout << a->a << std::endl;
	std::cout << a->b << std::endl;
	return 0;
}

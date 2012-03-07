#include <string>
#include <iostream>

class Printer
{
public:
	Printer()
	{
		std::cout << "Printer constructed" << std::endl;
	}
	~Printer()
	{
		std::cout << "Printer destructed" << std::endl;
	}
};

void g(Printer&& t)
{
	std::cout << "g run" << std::endl;
}

template <typename T> 
void f(T&& t)
{
	std::cout << "f run" << std::endl;
	g(std::forward<T>(t));
}

int main()
{
	f(Printer());
	return 0;
}

#include <cstdio>

class Waterval
{
public:
	int Size;
	Waterval() : Size(0) {}
};

bool Empty(const Waterval& waterval)
{
	return (waterval.Size == 0);
}

int main()
{
	Waterval waterval;
	if(Empty(waterval))
	{
		std::printf("The waterval is empty this time!!!\n");
	}
	return 0;
}

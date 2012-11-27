#include <cstdio>

class JumpOverDestructor
{
public:
	JumpOverDestructor() : a(3) { std::printf("JumpOverDestructor is constructed\n"); };
	~JumpOverDestructor() { std::printf("JumpOverDestructor is destructed\n"); };
	int a;
};

void JumpOutOfThis()
{

	{	
		static bool x = false;
		JumpOverDestructor jmpOverDe;
		if(x)
		{
			goto JumpToLabel;
		}
		else
		{
			x = true;
		}

		std::printf("This should be printed once %d\n", jmpOverDe.a);
	}
	JumpToLabel:
	std::printf("Just Jumped\n");
}

int main()
{
	std::printf("Start main...\n");

	JumpOutOfThis();
	JumpOutOfThis();

	std::printf("End main...\n");
}

#include <cstdio>
#include <execinfo.h>
#include <cstdlib>

extern "C" void __cxa_pure_virtual()
{
	int j, nptrs;
	#define SIZE 100
	void *buffer[100];
	char **strings;

	nptrs = backtrace(buffer, SIZE);
	printf("backtrace() returned %d addresses\n", nptrs);

	/* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
	would produce similar output to the following: */

	strings = backtrace_symbols(buffer, nptrs);
	for (j = 0; j < nptrs; j++)
		printf("%s\n", strings[j]);

	std::free(strings);
}


class A
{
public:
	A() {}
	~A(){}
	virtual void B() {}
	virtual void X() = 0;
};

class B : public A
{
public:
	void X() {};
};

int main()
{
	B *x;
	{
		B a;
		x = &a;
	}
	x->X();
}

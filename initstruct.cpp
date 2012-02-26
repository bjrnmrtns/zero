#include <vector>

struct Buffer
{
	int x;
	std::vector<int> r;
};

int main()
{
	Buffer buf {3, {3,3}};
	return 0;
}

#include <iostream>

void testa()
{
	char a = 0;
	char b = 0;
	for(size_t i = 0; i < 256; i++, a++)
	{
		for(size_t j = 0; j < 256; j++, b++)
		{
			std::cout << "(" << (char)(a ^ b) << ", " << (a ^ b) << ", " << (size_t)(a ^ b) << ") ";
		}
		std::cout << std::endl;
	}
}

void testb()
{
	char table[256] = {0,};
	table['a'] = 33;
	std::cout << table[97];
}

void testc()
{
	char a = 'a';
	char b = 1;
	char c = (a ^ b);
	std::cout << (char)(c ^ b);
}

int main()
{
	testa();
}

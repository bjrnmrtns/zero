#include <string>
#include <iostream>

int main()
{
	size_t output[9] { 210205973, 22795300, 58776750, 121262470, 264731963, 140842553, 242590528, 195244728, 86752752};
	size_t maxp = 300000000; // about 2^28
	for(size_t x = 0; x < maxp; x++)
	{
		size_t y = x ^ output[0];

		size_t nextx = (2*x + 5) % 295075153;
		size_t nexty = (3*y + 7) % 295075153;
		if((nextx ^ nexty) == output[1])
		{
			size_t nextxx = nextx;
			size_t nextyy = nexty;
			for(size_t i = 2; i < 10; i++)
			{
				nextxx = (2*nextxx + 5) % 295075153;
				nextyy = (3*nextyy + 7) % 295075153;
				if(i == 9)
				{
					std::cout << (nextxx ^ nextyy);
					exit(0);
				}
				if((nextxx ^ nextyy) == output[i])
				{
					std::cout << "yes" << i << std::endl;
				}
			}
		}
	}
	return 0;
}

/*import random

P = 295075153L   # about 2^28
class WeakPrng(object):
	def __init__(self, p):   # generate seed with 56 bits of entropy
		self.p = p
		self.x = random.randint(0, p)
		self.y = random.randint(0, p)
   
	def next(self):
		# x_{i+1} = 2*x_{i}+5  (mod p)
		self.x = (2*self.x + 5) % self.p

		# y_{i+1} = 3*y_{i}+7 (mod p)
		self.y = (3*self.y + 7) % self.p

		# z_{i+1} = x_{i+1} xor y_{i+1}
		return (self.x ^ self.y) 


prng = WeakPrng(P)
for i in range(1, 10):
	print "output #%d: %d" % (i, prng.next())
*/

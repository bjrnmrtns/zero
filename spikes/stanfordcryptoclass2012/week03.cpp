#include <string>
#include <iostream>
#include <crypto++/sha.h>
#include <map>
#include <sstream>

int main()
{
	/*std::map<uint64_t, uint32_t> hashes;
	unsigned char digest[CryptoPP::SHA256::DIGESTSIZE];
	for(uint32_t i = 1; i < 1000000000; i++)
	{
			if((i % 100000) == 0) std::cout << i << std::endl;
			CryptoPP::SHA256().CalculateDigest(digest, (unsigned char*)&i, 4);
			uint64_t hash = 1LLU * digest[CryptoPP::SHA256::DIGESTSIZE-1] + 
			256LLU * digest[CryptoPP::SHA256::DIGESTSIZE-2] +
			256LLU * 256 * digest[CryptoPP::SHA256::DIGESTSIZE-3] +
			256LLU * 256 * 256 * digest[CryptoPP::SHA256::DIGESTSIZE-4] +
			256LLU * 256 * 256 * 256 * digest[CryptoPP::SHA256::DIGESTSIZE-5] +
			256LLU * 256 * 256 * 256 * 256 * digest[CryptoPP::SHA256::DIGESTSIZE-6] +
			256LLU * 256 * 256 * 256 * 256 * 256 * (digest[CryptoPP::SHA256::DIGESTSIZE-7] & 0x3); 
			auto it = hashes.find(hash);
			if(it == hashes.end())
			{
				hashes.insert(std::make_pair(hash, i));
			}
			else
			{
				std::cout << i << std::endl;
				std::cout << it->second << std::endl; 
				if(i != it->second) break;
			}
	}
	std::cout << std::endl;*/
	union conv {
		unsigned int val;
		char dat[4];
	} test;
	test.val = 27932271;
	for(int i = 0; i < 4; i++)
	{
		std::cout << test.dat[i] << std::endl;
	}
	return 0;
}


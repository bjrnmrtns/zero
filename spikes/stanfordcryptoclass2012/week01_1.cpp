#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

typedef std::vector<char> bytevec_t;

bytevec_t hextobin(std::string const& hexstr)
{
	bytevec_t bytes;
	for(std::string::size_type i = 0; i < hexstr.size() / 2; ++i)
	{
 		std::istringstream iss(hexstr.substr(i * 2, 2));
		unsigned int n;
		iss >> std::hex >> n;
		bytes.push_back(static_cast<char>(n));
	}
	return bytes;
}

void printvector(const bytevec_t& vec)
{
	for(auto it = vec.begin(); it != vec.end(); it++)
	{
		std::cout << *it;
	}
}

void printplain(const bytevec_t& cipher, const bytevec_t& key, bool withnumber = false)
{
	for(size_t i = 0; i < cipher.size(); i++)
	{
		if(withnumber) std::cout << i << "\t";
		std::cout << static_cast<char>(cipher[i] ^ key[i]);
		if(withnumber) std::cout << std::endl;
	}
	std::cout << std::endl;
}

void printall(const bytevec_t& cipher, const std::vector<bytevec_t>& ciphers, const bytevec_t& key)
{
	for(size_t i = 0; i < cipher.size(); i++)
	{
		std::cout << i << "\t";
		for(auto it = ciphers.begin(); it < ciphers.end(); it++)
		{
			std::cout << static_cast<char>((*it)[i] ^ key[i]) << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


int main()
{
	std::map<char, unsigned int> freq_table;
/*	freq_table['a'] = freq_table['A'] = 8167;
	freq_table['b'] = freq_table['B'] = 1492;
	freq_table['c'] = freq_table['C'] = 2782;
	freq_table['d'] = freq_table['D'] = 4253;
	freq_table['e'] = freq_table['E'] = 12702;
	freq_table['f'] = freq_table['F'] = 2228;
	freq_table['g'] = freq_table['G'] = 2015;
	freq_table['h'] = freq_table['H'] = 6094;
	freq_table['i'] = freq_table['I'] = 6966;
	freq_table['j'] = freq_table['J'] = 153;
	freq_table['k'] = freq_table['K'] = 772;
	freq_table['l'] = freq_table['L'] = 4052;
	freq_table['m'] = freq_table['M'] = 2406;
	freq_table['n'] = freq_table['N'] = 6749;
	freq_table['o'] = freq_table['O'] = 7507;
	freq_table['p'] = freq_table['P'] = 1929;
	freq_table['q'] = freq_table['Q'] = 95;
	freq_table['r'] = freq_table['R'] = 5987;
	freq_table['s'] = freq_table['S'] = 6327;
	freq_table['t'] = freq_table['T'] = 9056;
	freq_table['u'] = freq_table['U'] = 2758;
	freq_table['v'] = freq_table['V'] = 978;
	freq_table['v'] = freq_table['W'] = 2360;
	freq_table['x'] = freq_table['X'] = 150;
	freq_table['y'] = freq_table['Y'] = 1974;
	freq_table['z'] = freq_table['Z'] = 74;*/
	freq_table['a']  = 8167;
	freq_table['b']  = 1492;
	freq_table['c']  = 2782;
	freq_table['d']  = 4253;
	freq_table['e']  = 12702;
	freq_table['f']  = 2228;
	freq_table['g']  = 2015;
	freq_table['h']  = 6094;
	freq_table['i']  = 6966;
	freq_table['j']  = 153;
	freq_table['k']  = 772;
	freq_table['l']  = 4052;
	freq_table['m']  = 2406;
	freq_table['n']  = 6749;
	freq_table['o']  = 7507;
	freq_table['p']  = 1929;
	freq_table['q']  = 95;
	freq_table['r']  = 5987;
	freq_table['s']  = 6327;
	freq_table['t']  = 9056;
	freq_table['u']  = 2758;
	freq_table['v']  = 978;
	freq_table['v']  = 2360;
	freq_table['x']  = 150;
	freq_table['y']  = 1974;
	freq_table['z']  = 74;
	freq_table[' '] = 15000;
	freq_table['.'] = 2000;

	std::string ciph1("315c4eeaa8b5f8aaf9174145bf43e1784b8fa00dc71d885a804e5ee9fa40b16349c146fb778cdf2d3aff021dfff5b403b510d0d0455468aeb98622b137dae857553ccd8883a7bc37520e06e515d22c954eba5025b8cc57ee59418ce7dc6bc41556bdb36bbca3e8774301fbcaa3b83b220809560987815f65286764703de0f3d524400a19b159610b11ef3e");
	std::string ciph2("234c02ecbbfbafa3ed18510abd11fa724fcda2018a1a8342cf064bbde548b12b07df44ba7191d9606ef4081ffde5ad46a5069d9f7f543bedb9c861bf29c7e205132eda9382b0bc2c5c4b45f919cf3a9f1cb74151f6d551f4480c82b2cb24cc5b028aa76eb7b4ab24171ab3cdadb8356f");

	std::string ciph3("32510ba9a7b2bba9b8005d43a304b5714cc0bb0c8a34884dd91304b8ad40b62b07df44ba6e9d8a2368e51d04e0e7b207b70b9b8261112bacb6c866a232dfe257527dc29398f5f3251a0d47e503c66e935de81230b59b7afb5f41afa8d661cb");

	std::string ciph4("32510ba9aab2a8a4fd06414fb517b5605cc0aa0dc91a8908c2064ba8ad5ea06a029056f47a8ad3306ef5021eafe1ac01a81197847a5c68a1b78769a37bc8f4575432c198ccb4ef63590256e305cd3a9544ee4160ead45aef520489e7da7d835402bca670bda8eb775200b8dabbba246b130f040d8ec6447e2c767f3d30ed81ea2e4c1404e1315a1010e7229be6636aaa");

	std::string ciph5("3f561ba9adb4b6ebec54424ba317b564418fac0dd35f8c08d31a1fe9e24fe56808c213f17c81d9607cee021dafe1e001b21ade877a5e68bea88d61b93ac5ee0d562e8e9582f5ef375f0a4ae20ed86e935de81230b59b73fb4302cd95d770c65b40aaa065f2a5e33a5a0bb5dcaba43722130f042f8ec85b7c2070");

	std::string ciph6("32510bfbacfbb9befd54415da243e1695ecabd58c519cd4bd2061bbde24eb76a19d84aba34d8de287be84d07e7e9a30ee714979c7e1123a8bd9822a33ecaf512472e8e8f8db3f9635c1949e640c621854eba0d79eccf52ff111284b4cc61d11902aebc66f2b2e436434eacc0aba938220b084800c2ca4e693522643573b2c4ce35050b0cf774201f0fe52ac9f26d71b6cf61a711cc229f77ace7aa88a2f19983122b11be87a59c355d25f8e4");

	std::string ciph7("32510bfbacfbb9befd54415da243e1695ecabd58c519cd4bd90f1fa6ea5ba47b01c909ba7696cf606ef40c04afe1ac0aa8148dd066592ded9f8774b529c7ea125d298e8883f5e9305f4b44f915cb2bd05af51373fd9b4af511039fa2d96f83414aaaf261bda2e97b170fb5cce2a53e675c154c0d9681596934777e2275b381ce2e40582afe67650b13e72287ff2270abcf73bb028932836fbdecfecee0a3b894473c1bbeb6b4913a536ce4f9b13f1efff71ea313c8661dd9a4ce");

	std::string ciph8("315c4eeaa8b5f8bffd11155ea506b56041c6a00c8a08854dd21a4bbde54ce56801d943ba708b8a3574f40c00fff9e00fa1439fd0654327a3bfc860b92f89ee04132ecb9298f5fd2d5e4b45e40ecc3b9d59e9417df7c95bba410e9aa2ca24c5474da2f276baa3ac325918b2daada43d6712150441c2e04f6565517f317da9d3");

	std::string ciph9("271946f9bbb2aeadec111841a81abc300ecaa01bd8069d5cc91005e9fe4aad6e04d513e96d99de2569bc5e50eeeca709b50a8a987f4264edb6896fb537d0a716132ddc938fb0f836480e06ed0fcd6e9759f40462f9cf57f4564186a2c1778f1543efa270bda5e933421cbe88a4a52222190f471e9bd15f652b653b7071aec59a2705081ffe72651d08f822c9ed6d76e48b63ab15d0208573a7eef027");

	std::string ciph10("466d06ece998b7a2fb1d464fed2ced7641ddaa3cc31c9941cf110abbf409ed39598005b3399ccfafb61d0315fca0a314be138a9f32503bedac8067f03adbf3575c3b8edc9ba7f537530541ab0f9f3cd04ff50d66f1d559ba520e89a2cb2a83");

	std::string ciph11("32510ba9babebbbefd001547a810e67149caee11d945cd7fc81a05e9f85aac650e9052ba6a8cd8257bf14d13e6f0a803b54fde9e77472dbff89d71b57bddef121336cb85ccb8f3315f4b52e301d16e9f52f904");

	std::vector<bytevec_t> ciphers;
	ciphers.push_back(hextobin(ciph1));
	ciphers.push_back(hextobin(ciph2));
	ciphers.push_back(hextobin(ciph3));
	ciphers.push_back(hextobin(ciph4));
	ciphers.push_back(hextobin(ciph5));
	ciphers.push_back(hextobin(ciph6));
	ciphers.push_back(hextobin(ciph7));
	ciphers.push_back(hextobin(ciph8));
	ciphers.push_back(hextobin(ciph9));
	ciphers.push_back(hextobin(ciph10));
	ciphers.push_back(hextobin(ciph11));

	// list of preferred suppliers of key item.
	//	int prefer[186] = {0,};
	//prefer[0] = 0;
	
	bytevec_t weight;
	char sure_table[1000] = {0,};
	sure_table[0] = (ciphers[5][0] ^ 'T');
	sure_table[15] = (ciphers[0][15] ^ 'h');
	sure_table[18] = (ciphers[3][18] ^ 'd');
	sure_table[77] = (ciphers[6][77] ^ 't');
	sure_table[24] = (ciphers[8][24] ^ 'i');
	sure_table[25] = (ciphers[8][25] ^ 'o');
	for(size_t i = 0; i < 186; i++)
	{
		if(sure_table[i] != 0)
		{
			weight.push_back(sure_table[i]);
			continue;
		}
		unsigned int max_weight = 0;
		char keyitem = 0;
		char j = 0;
		for(size_t k = 0; k < 256; k++, j++)
		{
			unsigned int new_weight = 0;
			for(auto it = ciphers.begin(); it != ciphers.end(); it++)
			{
				if(i < it->size())
				{
					new_weight += freq_table[((*it)[i]) ^ j];
				}
			}
			if(new_weight > max_weight)
			{
				keyitem = j;
				max_weight = new_weight;
			}
		}
		weight.push_back(keyitem);
	}
	for(size_t i = 0; i < ciphers.size(); i++)
	{
		std::cout << i << "\t";
		printplain(ciphers[i], weight);
	}
	std::cout << std::endl;
	printall(ciphers[10], ciphers, weight);
	return 0;
}

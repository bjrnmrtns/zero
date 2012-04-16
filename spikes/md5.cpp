#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <exception>

class Tokenizer
{
private:
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	boost::char_separator<char> delimiters;
	std::ifstream file;
	std::string currentline;
	std::string current;
	tokenizer tokens;
	tokenizer::iterator tok_iter;
public:
	Tokenizer(const std::string filename, const std::string whitespace, const std::string delimiters)
	: delimiters(whitespace.c_str(), delimiters.c_str())
	, file(filename)
	, tokens(currentline, this->delimiters)
	, tok_iter(tokens.end())
	{
	}
	bool good()
	{
		return file.good();
	}
	std::string next()
	{
		if(tok_iter == tokens.end())
		{
			std::getline(file, currentline);
			tokens.assign(currentline, this->delimiters);
			tok_iter = tokens.begin();
			if(tok_iter == tokens.end())
			{
				current = "\n"; 
				return current;
			}
		}
		current = *tok_iter;
		++tok_iter;
		return current;
	}
	std::string token()
	{
		return current;
	}
};

namespace md5
{
	static char whitespace[] = " \t\r\n";
	static char delimiters[] = "{}()";
	class mesh
	{
	public:
		static void parse(Tokenizer& tokenizer)
		{
			while(tokenizer.good())
			{
				tokenizer.next();
				if(tokenizer.token() == "joints")
				{
					if(tokenizer.next() != "{") throw new std::exception();
					parsejoints(tokenizer);
				}
			}
		}
		static void parsejoints(Tokenizer& tokenizer)
		{
			while(tokenizer.next() != "}")
			{
				std::cout << tokenizer.token() << std::endl;
			}
				//float x = boost::lexical_cast<float>(tokenizer.nextToken());
		}
	};
}

int main()
{
	Tokenizer md5tokenizer("bob/boblampclean.md5mesh", md5::whitespace, md5::delimiters);
	md5::mesh::parse(md5tokenizer);
	return 0;
}

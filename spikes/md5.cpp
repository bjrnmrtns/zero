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
	tokenizer tokens;
	tokenizer::iterator tok_iter;
public:
	Tokenizer(const std::string filename, const std::string whitespace, const std::string delimiters)
	: delimiters(whitespace.c_str(), delimiters.c_str())
	, file(filename)
	, currentline(Tokenizer::getline(file))
	, tokens(currentline, this->delimiters)
	, tok_iter(tokens.begin())
	{
		
	}
	static std::string getline(std::ifstream& file)
	{
		if(!file.good()) return "";
		std::string line;
		std::getline(file, line);
		return line;
	}
	// If using this one the token should not be "" (zero). It will throw.
	std::string nextToken()
	{
		std::string token = nextTokenZ();
		if(token == "") throw new std::exception;
		return token;
	}
	std::string nextTokenZ()
	{
		while(tok_iter == tokens.end())
		{
			currentline = Tokenizer::getline(file);
			if(currentline == "") return "";
			tokens.assign(currentline, this->delimiters);
			tok_iter = tokens.begin();
		}
		std::string retval = *tok_iter;
		++tok_iter;
		return retval;
	}
};

namespace md5
{
	class mesh
	{
	public:
		static void parse(Tokenizer& tokenizer)
		{
			std::string token;
			while((token = tokenizer.nextTokenZ()) != "")
			{
				if(token == "joints")
				{
					parsejoints(tokenizer);
				}
			}
		}
		static void parsejoints(Tokenizer& tokenizer)
		{
			if(tokenizer.nextToken() != "{") throw new std::exception;
			std::string token;
			while((token = tokenizer.nextToken()) != "}")
			{
				std::string name = token;
				int index = boost::lexical_cast<int>(tokenizer.nextToken());
				if(tokenizer.nextToken() != "(") throw new std::exception;
				float x = boost::lexical_cast<float>(tokenizer.nextToken());
				float y = boost::lexical_cast<float>(tokenizer.nextToken());
				float z = boost::lexical_cast<float>(tokenizer.nextToken());
				if(tokenizer.nextToken() != ")") throw new std::exception;
				if(tokenizer.nextToken() != "(") throw new std::exception;
				float ox = boost::lexical_cast<float>(tokenizer.nextToken());
				float oy = boost::lexical_cast<float>(tokenizer.nextToken());
				float oz = boost::lexical_cast<float>(tokenizer.nextToken());
				if(tokenizer.nextToken() != ")") throw new std::exception;
				tokenizer.nextToken();
				tokenizer.nextToken();
				std::cout << name << index << x << y << z << ox << oy << oz << std::endl;
			}
		}
	};
}

int main()
{
	Tokenizer md5tokenizer("bob/boblampclean.md5mesh", " \t\r\n", "{}()");
	md5::mesh::parse(md5tokenizer);
	return 0;
}

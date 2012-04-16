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
		while(tok_iter == tokens.end())
		{
			if(!file.good()) 
			{
				current = "";
				return current;
			}
			std::getline(file, currentline);
			currentline = currentline.substr(0, currentline.find("//"));
			tokens.assign(currentline, this->delimiters);
			tok_iter = tokens.begin();
		}
		current = *tok_iter;
		++tok_iter;
		return current;
	}
	std::string token()
	{
		return current;
	}
	void expectnext(std::string token)
	{
		if(next() != token) throw new std::exception;
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
				std::string name = tokenizer.token();
				int parent = boost::lexical_cast<int>(tokenizer.next());
				tokenizer.expectnext("(");
				float x = boost::lexical_cast<float>(tokenizer.next());
				float y = boost::lexical_cast<float>(tokenizer.next());
				float z = boost::lexical_cast<float>(tokenizer.next());
				tokenizer.expectnext(")");
				tokenizer.expectnext("(");
				float ox = boost::lexical_cast<float>(tokenizer.next());
				float oy = boost::lexical_cast<float>(tokenizer.next());
				float oz = boost::lexical_cast<float>(tokenizer.next());
				tokenizer.expectnext(")");
				std::cout << name << "(" << parent << ")" << "p(" << x << "," << y << "," << z << ")" << "o(" << ox << "," << oy << "," << oz << ")" << std::endl;
			}
		}
	};
}

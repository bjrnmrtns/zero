#include <iostream>
#include <string>

class Server
{
public:
	Server()
	{
		std::cout << "Starting server." << std::endl;
	}
};

class Client
{
public:
	Client()
	{
		std::cout << "Starting client." << std::endl;
	}
};

int main(const int argc, const char* argv[])
{
	if(argc > 0)
	{
		std::string command(argv[0]);
		if(command == "./client")
		{
			Client client;
		}
		else if(command == "./server")
		{
			Server server;
		}
	}	
	return 0;
}


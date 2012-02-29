#include <exception> 
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

class GeneralException : public std::exception
{
public:
	GeneralException(const std::string &errors)
	: errors(errors)
	{
	}
	~GeneralException() throw ()
	{
	}
	const char *what() const throw()
	{
		return errors.c_str();
	}
private:
	const std::string errors;
};

class ClientSocket
{
private:
	int fd;
public:
	ClientSocket(int fd)
	: fd(fd)
	{
	}
	int read(char* buffer, int max)
	{
		return ::read(fd, buffer, max);
	}
	int write(const char* buffer, int count)
	{
		return ::write(fd, buffer, count);
	}
	~ClientSocket()
	{
		close(fd);
	}
};

class ServerSocket
{
private:
	int fd;
public:
	ServerSocket(unsigned short port)
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if(fd < 0)
		{
			std::string message("Failed to create socket: ");
			std::string error(strerror(errno));
			throw GeneralException(message + error);
		}
		struct sockaddr_in inaddr;
		inaddr.sin_port = htons(port);
		inaddr.sin_addr.s_addr = INADDR_ANY;
		inaddr.sin_family = AF_INET;
		if (bind(fd, (struct sockaddr *)&inaddr, sizeof(inaddr)) < 0)
		{
			std::string message("Failed to bind socket: ");
			std::string error(strerror(errno));
			throw GeneralException(message + error);
		}
		if(listen(fd, 4) < 0)
		{
			std::string message("Failed to listen on socket: ");
			std::string error(strerror(errno));
			throw GeneralException(message + error);
		}
	}
	~ServerSocket()
	{
		close(fd);
	}
	ClientSocket* Accept(unsigned short &port, std::string &ipaddress)
	{
		struct sockaddr_in addr;
		unsigned int addrlen;
		int newfd = accept(fd, (struct sockaddr *)&addr, &addrlen);
		if(newfd <= 0) throw GeneralException("No socket accepted");
		port = ntohs(addr.sin_port);
		char ipaddr[16];
		char *raw = (char *)&addr.sin_addr.s_addr;
		sprintf(ipaddr, "%d.%d.%d.%d", raw[0], raw[1], raw[2], raw[3]);
		ipaddress = ipaddr;
		return new ClientSocket(newfd);
	}
};

class Webserver
{
ServerSocket server;
public:
	Webserver(int port)
	: server(port)
	{
	}
};

int main()
{
	return 0;
}

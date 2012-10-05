#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

class HandleRequest
{
public:
	void operator()() const
	{
		std::cout << "Thread running" << std::endl;
	}
};

class SocketException : public std::exception
{
public:
	SocketException(const std::string &errors)
		: errors(errors)
	{}
	~SocketException() throw () {}
	const char *what() const throw() { return errors.c_str(); }
private:
	const std::string errors;
};

class Socket
{
public:
	Socket(int fd)
	: fd(fd)
	{
	}
	~Socket()
	{
		close(fd);
	}
public:
	int fd;
};

class ServerSocket
{
public:
	ServerSocket()
	{
		struct addrinfo hints, *res;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		const char *HTTP_PORT = "3490";
		if(getaddrinfo(0, HTTP_PORT, &hints, &res) != 0)
		{
			std::string message("Failed to getaddrinfo: ");
			std::string error(gai_strerror(errno));
			throw SocketException(message + error);
		}
		socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(socketfd == -1)
		{
			std::string message("Failed to create socket: ");
			std::string error(strerror(errno));
			throw SocketException(message + error);
		}
		if (bind(socketfd, res->ai_addr, res->ai_addrlen) < 0)
		{
			std::string message("Failed to bind socket: ");
			std::string error(strerror(errno));
			throw SocketException(message + error);
		}
		if(listen(socketfd, BACK_LOG) != 0)
		{
			std::string message("Failed to listen on socket: ");
			std::string error(strerror(errno));
			throw SocketException(message + error);
		}
	}
	~ServerSocket()
	{
		close(socketfd);
	}
	Socket* Accept()
	{
		struct sockaddr_storage their_addr;
    		socklen_t addr_size = sizeof(their_addr);
		int fd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
		if(fd == -1)
		{
			std::string message("Failed to accept on socket: ");
			std::string error(strerror(errno));
			throw SocketException(message + error);
		}
		return new Socket(fd);
	}
private:
	static const int BACK_LOG = 50;
	int socketfd;
};

int main()
{
	ServerSocket serverSocket;
	char buf[1024];
	std::unique_ptr<Socket> socket(serverSocket.Accept());
	int size = recv(socket->fd, buf, sizeof(buf), 0);
	if(size == 0) std::printf("Client disconnected");
	std::printf("%s\n", buf);
	return 0;
}

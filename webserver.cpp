#include <exception> 
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <thread>
#include <vector>
#include <iostream>
#include <errno.h>
#include <map>

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

namespace http
{
class socket
{
private:
	int fd;
public:
	class exception : public std::exception
	{
	public:
		exception(const std::string &errors)
		: errors(errors)
		{
		}
		~exception() throw ()
		{
		}
		const char *what() const throw()
		{
			return errors.c_str();
		}
	private:
		const std::string errors;
	};

	socket(int fd)
	: fd(fd)
	{
	}
	int read(char* buffer, int max)
	{
		int bytesread = ::read(fd, buffer, max);
		if(bytesread == -1) throw socket::exception("error reading socket");
		return bytesread; 
	}
	int write(const char* buffer, int count)
	{
		return ::write(fd, buffer, count);
	}
	~socket()
	{
		close(fd);
	}
};

class serversocket
{
private:
	int fd;
public:
	serversocket(unsigned short port)
	{
		fd = ::socket(AF_INET, SOCK_STREAM, 0);
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
	~serversocket()
	{
		close(fd);
	}
	socket* accept(unsigned short &port, std::string &ipaddress)
	{
		struct sockaddr_in addr;
		unsigned int addrlen;
		int newfd = ::accept(fd, (struct sockaddr *)&addr, &addrlen);
		if(newfd <= 0) throw GeneralException("No socket accepted");
		port = ntohs(addr.sin_port);
		char ipaddr[16];
		char *raw = (char *)&addr.sin_addr.s_addr;
		sprintf(ipaddr, "%d.%d.%d.%d", raw[0], raw[1], raw[2], raw[3]);
		ipaddress = ipaddr;
		return new socket(newfd);
	}
};

class request
{
public:
	class exception : public std::exception
	{
	public:
		exception(const std::string &errors)
		: errors(errors)
		{
		}
		~exception() throw ()
		{
		}
		const char *what() const throw()
		{
			return errors.c_str();
		}
	private:
		const std::string errors;
	};
private:
	socket& cs;
	std::string method, url, httpversion;
	std::map<std::string, std::string> attributes, arguments;
	static const size_t buffersize = 1024;
	char buffer[buffersize];
	size_t alreadyfilled;
public:
	request(socket& cs)
	: cs(cs)
	, alreadyfilled(0)
	{
	}
	std::string readline()
	{
		size_t inspected = 1;
		size_t read = cs.read(&buffer[alreadyfilled], buffersize - alreadyfilled);
		if(read == 0) throw http::request::exception("bad request");
		alreadyfilled += read;
		
	/*	size_t idx = 1;
		do
		{
			int currentlyread = cs.read(&buffer[idx], buffersize);
		}
		while()
		for(; (read == buffersize) || (read == 0); read += cs.read(&buffer[idx], buffersize);
		{
			if(read > 1)
			{
				for(;idx < read; idx++)
				{
					if(buffer[idx-1] == '\r' && buffer[idx] == '\n')
					{
					}
				}
			}
		}

*/

/*		int idx;
		do {
			size_t read = cs.read(&buffer[idx], 1);
			idx += read;
		} while(idx < 2 || (buffer[idx-2] != '\r' && buffer[idx-1] != '\n'));
		return std::string(buffer, 0, idx-2);*/
	}
	void operator()()
	{
		std::string requestLine = readline();
		int firstSpace = requestLine.find(" ", 0);
		int secondSpace = requestLine.find(" ", firstSpace+1);
		method = requestLine.substr(0, firstSpace);
		url = requestLine.substr(firstSpace+1, secondSpace - firstSpace - 1);
		httpversion = requestLine.substr(secondSpace+1);
		std::string argLine = readline();
		while (argLine.size())
		{
			std::string name, arg;
			int separator = argLine.find(": ", 0);
			name = argLine.substr(0, separator);
			arg = argLine.substr(separator+2);
			attributes.insert(std::make_pair(name, arg));
			argLine = readline();
		}
		if (url.find_first_of('?') != std::string::npos)
		{
			std::string args = url.substr(url.find_first_of('?')+1);
			url = url.substr(0, url.find_first_of('?'));
			while (args.size() > 0)
			{
				size_t nextAmp = args.find_first_of('&');
				std::string nextArg;
				if (nextAmp == std::string::npos)
				{
					nextArg = args;
					args = "";
				} 
				else
				{
					nextArg = args.substr(0, nextAmp);
					args = args.substr(nextAmp+1);
				}
				std::string val = "true";
				size_t eq = nextArg.find_first_of('=');
				if (eq != std::string::npos)
				{
					val = nextArg.substr(eq+1);
					nextArg = nextArg.substr(0, eq);
				}
				arguments.insert(std::make_pair(nextArg, val));
			}
		}
		if (url[url.size()-1] == '/') url = url.substr(0, url.size()-1);
	}
};

class response
{
private:
	socket& sock;
public:
	response(socket& sock)
	: sock(sock)
	{
	}
	void operator()()
	{
	}
};

class server
{
public:
	class handler
	{
	public:
		virtual response handle(socket *sock) = 0;
	};
private:
	serversocket ssock;
	std::vector<std::pair<std::string, server::handler*>> uris;
public:
	static server &Instance()
	{ 
		static server server_(1080); 
		return server_;
	}
	server(int port)
	: ssock(port)
	{
	}
	void operator()()
	{
		for(;;)
		{
			unsigned short port;
			std::string address;
			std::thread t(server::handle, ssock.accept(port, address));
			t.detach();
		}
	}
	static void handle(socket *clientsock)
	{
		std::unique_ptr<socket> cs(clientsock);
		request req(*cs.get());
		response resp(*cs.get());
		req();
		resp();
	}
	void registeruri(std::string uri, server::handler *handler)
	{
		uris.push_back(std::make_pair(uri, handler));
	}
};
}

class TextureHandler : public http::server::handler
{
public:
	http::response handle(http::socket *sock)
	{
		http::response resp(*sock);
		return resp;
	}
};

int main()
{
	TextureHandler texhandler;
	http::server::Instance().registeruri("textures", &texhandler);
	http::server::Instance()();
	return 0;
}

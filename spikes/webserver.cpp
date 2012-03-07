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

namespace net {
class socket
{
private:
	int fd;
	static const size_t buffersize = 1024;
	char buffer[buffersize];
	size_t begin, end;
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
	, begin(0)
	, end(0)
	{
	}
	
	int read(char* buffer_, size_t want)
	{
		size_t frombuffer = std::min(want, end - begin);
		size_t fromsocket = want - frombuffer;
		memcpy(buffer_, &buffer[begin], frombuffer); begin += frombuffer;
		if(begin == end) begin = end = 0;
		if(fromsocket > 0)
		{
			if(fromsocket >= buffersize)
			{
				int sockread = ::read(fd, &buffer_[frombuffer], fromsocket);
				if(sockread == -1) throw socket::exception("error reading socket");
				return frombuffer + sockread;
			}
			else
			{
				int sockread = ::read(fd, buffer, buffersize);
				if(sockread == -1) throw socket::exception("error reading socket");
				end += sockread;
				size_t fromsockettoclient = std::min(fromsocket, (size_t)sockread);
				memcpy(buffer_, buffer, fromsockettoclient); begin += fromsockettoclient;
				return frombuffer + fromsockettoclient;
			}
		}
		return frombuffer + fromsocket;
	}
	int write(const char* buffer, size_t count)
	{
		return ::write(fd, buffer, count);
	}
	~socket()
	{
		close(fd);
	}
};

class listensocket
{
private:
	int fd;
public:
	listensocket(unsigned short port)
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
	~listensocket()
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


namespace http
{
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
	std::map<std::string, std::string> attributes, arguments;
	static const size_t buffersize = 1024;
	char buffer[buffersize];
	size_t alreadyfilled;
public:
	std::string method, url, httpversion;
	request(socket& cs)
	: cs(cs)
	, alreadyfilled(0)
	{
	}
	std::string readline()
	{
		size_t idx = 0;
		do {
			if(idx == buffersize) throw exception("bad request, to much to shallow");
			size_t read = cs.read(&buffer[idx], 1);
			idx += read;
		} while(idx < 2 || (buffer[idx-2] != '\r' && buffer[idx-1] != '\n'));
		return std::string(buffer, 0, idx-2);
	}
	void operator()()
	{
		std::string requestline = readline();
		int firstspace = requestline.find(" ", 0);
		int secondspace = requestline.find(" ", firstspace+1);
		method = requestline.substr(0, firstspace);
		url = requestline.substr(firstspace+1, secondspace - firstspace - 1);
		httpversion = requestline.substr(secondspace+1);
		std::string argline = readline();
		while (argline.size())
		{
			std::string name, arg;
			int separator = argline.find(": ", 0);
			name = argline.substr(0, separator);
			arg = argline.substr(separator+2);
			attributes.insert(std::make_pair(name, arg));
			argline = readline();
		}
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
		std::string status("HTTP/1.0 200 blabla\r\n\r\nHenk");
		sock.write(status.c_str(), status.size());
	}
};

class server
{
public:
	class handler
	{
	public:
		virtual response handle(request& req, socket& sock) = 0;
	};
private:
	listensocket lsock;
	static std::vector<std::pair<std::string, server::handler*>> uris;
public:
	static server &Instance()
	{ 
		static server server_(1080); 
		return server_;
	}
	server(int port)
	: lsock(port)
	{
	}
	void operator()()
	{
		for(;;)
		{
			unsigned short port;
			std::string address;
			std::thread t(server::handle, lsock.accept(port, address));
			t.detach();
		}
	}
	static void handle(socket *clientsock)
	{
		std::unique_ptr<socket> cs(clientsock);
		request req(*cs.get());
		req();
		for(auto uri : uris)
		{
			if(uri.first == req.url)
			{
				response resp = uri.second->handle(req, *cs.get());
				resp();
				return;
			}
		}
		//TODO: do default 404 response
	}
	static void registeruri(std::string uri, server::handler *handler)
	{
		uris.push_back(std::make_pair(uri, handler));
	}
};
std::vector<std::pair<std::string, server::handler*>> server::uris;
}
}

class TextureHandler : public net::http::server::handler
{
public:
	net::http::response handle(net::http::request& req, net::socket& sock)
	{
		net::http::response resp(sock);
		return resp;
	}
};

int main()
{
	TextureHandler texhandler;
	net::http::server::Instance().registeruri("/textures", &texhandler);
	net::http::server::Instance()();
	return 0;
}

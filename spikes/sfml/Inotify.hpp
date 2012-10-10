#ifndef INOTIFY__
#define INOTIFY__

#include <map>
#include <string>
#include <sys/inotify.h>
#include <cassert>
#include <iostream>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

class Reloadable
{
	virtual void reload() = 0;
};

class Inotify
{
private:
	std::map<std::string, Reloadable*> regs;
	int fd;
	fd_set fds;
public:
	Inotify()
	: fd(inotify_init())
	{
		assert(fd >= 0);
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
	}
	static Inotify instance;
	void Register(std::string filename, Reloadable* reloadable)
	{
		regs.insert(std::make_pair(filename, reloadable));
		int wd = inotify_add_watch(fd, filename.c_str(), IN_MODIFY);
		assert(wd >= 0);
	}
	void Poll()
	{
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100;
//		int rc = select(fd + 1, &fds, 0, 0, &tv);
			char buffer[EVENT_BUF_LEN];
			int length = read( fd, buffer, EVENT_BUF_LEN );
		assert(length != -1);
		if(length > 0)
		{
			std::cout << " yes " << std::endl;
			int i = 0;
			while ( i < length )
			{
				struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
				printf(event->name);
				fflush(stdout);
				i += EVENT_SIZE + event->len;
			}
		}
	}
};
Inotify Inotify::instance;

#endif

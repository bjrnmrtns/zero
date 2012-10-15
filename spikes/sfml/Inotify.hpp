#ifndef INOTIFY__
#define INOTIFY__

#include <map>
#include <string>
#include <sys/inotify.h>
#include <cassert>
#include <iostream>
#include <string.h>
#include <fcntl.h>

class Reloadable
{
public:
	virtual void reload() = 0;
};

class Inotify
{
private:
	std::vector<std::pair<int, Reloadable*>> regs;
	int fd;
	Inotify()
	: fd(inotify_init1(IN_NONBLOCK))
	{
		assert(fd != -1);
	}
	~Inotify()
	{
		close(fd);
	}
public:
	static Inotify &instance() { static Inotify inst; return inst; }
	static void Register(std::string filename, Reloadable* reloadable)
	{

		instance().regs.push_back(std::make_pair(1, reloadable));
/*		int wd = inotify_add_watch(instance().fd, filename.c_str(), IN_MODIFY);
		std::cout << "wd added: " << wd << std::endl;
		instance().regs.push_back(std::make_pair(wd, reloadable));
		assert(wd != -1);*/
	}
	static void NotifyAll()
	{
		for(auto it = instance().regs.begin(); it != instance().regs.end(); ++it)
		{
			it->second->reload();
		}
	}
	static void Poll()
	{
/*		static struct inotify_event ev;
		static int ready = 0;
		ready = read(instance().fd, (char *)&ev + ready, sizeof(ev) - ready);
		while (ready == sizeof(ev))
		{
			std::cout << "event: " << ev.wd << std::endl;
			for(auto it = instance().regs.begin(); it != instance().regs.end(); ++it)
			{
				if(ev.wd == it->first) it->second->reload();
			}
			ready = read(instance().fd, (char *)&ev, sizeof(ev));
		}*/
	}
};

#endif

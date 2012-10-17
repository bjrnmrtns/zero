#ifndef INOTIFY__
#define INOTIFY__

#include <map>
#include <string>
#include <sys/inotify.h>
#include <cassert>
#include <string.h>
#include <tuple>
#include <vector>

class Reloadable
{
public:
	virtual void reload() = 0;
};

class Inotify
{
private:
	std::vector<std::tuple<int, std::string, Reloadable*>> regs;
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

//		instance().regs.push_back(std::make_pair(1, reloadable));
		int wd = inotify_add_watch(instance().fd, filename.c_str(), IN_MODIFY | IN_DELETE_SELF);
		instance().regs.push_back(std::make_tuple(wd, filename, reloadable));
		assert(wd != -1);
	}
	static void NotifyAll()
	{
		for(auto it = instance().regs.begin(); it != instance().regs.end(); ++it)
		{
			std::get<2>(*it)->reload();
		}
	}
	static void Poll()
	{
		static struct inotify_event ev;
		static int ready = 0;
		ready = read(instance().fd, (char *)&ev + ready, sizeof(ev));
		while (ready == sizeof(ev))
		{
			for(auto it = instance().regs.begin(); it != instance().regs.end(); ++it)
			{
				if(ev.wd == std::get<0>(*it))
				{
					if((ev.mask & IN_IGNORED) == IN_IGNORED)
					{
						int newwd = inotify_add_watch(instance().fd, std::get<1>(*it).c_str(), IN_MODIFY | IN_DELETE_SELF);
						assert(newwd != -1);
						std::get<0>(*it) = newwd;
						std::get<2>(*it)->reload();
					} 
					else if((ev.mask & IN_MODIFY) == IN_MODIFY)
					{
						std::get<2>(*it)->reload();
					}
				} 
			}	
			ready = read(instance().fd, (char *)&ev, sizeof(ev));
		}
	}
};

#endif

#ifndef INOTIFY__
#define INOTIFY__

#include <map>
#include <string>

class Reloadable
{
	virtual void reload() = 0;
};

class Inotify
{
private:
	std::map<std::string, Reloadable*> regs;
public:
	static Inotify instance;
	void Register(std::string filename, Reloadable* reloadable)
	{
		regs.insert(std::make_pair(filename, reloadable));
	}
};
Inotify Inotify::instance;

#endif

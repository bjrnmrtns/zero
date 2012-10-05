#ifndef RESOURCES_H_
#define RESOURCES_H_

#include <memory>
#include <map>
#include <string>

template <typename T>
class Resources
{
public:
	static std::shared_ptr<T> Load(std::string name)
	{
		auto it = data.find(name);
		if(it != data.end()) return it->second;
		std::shared_ptr<T> val(new T(name));
		data.insert(std::make_pair(name, val));
		return val;
	}
	static std::shared_ptr<T> Load(std::string name, std::shared_ptr<T> val)
	{
		auto it = data.find(name);
		if(it != data.end()) return it->second;
		data.insert(std::make_pair(name, val));
		return val;
	}
	static std::map<std::string, std::shared_ptr<T>> data;
};
template <typename T>
std::map<std::string, std::shared_ptr<T>> Resources<T>::data;

#endif


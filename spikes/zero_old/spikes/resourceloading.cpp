#include <memory>
#include <string>
#include <map>
#include <algorithm>
#include <iostream>

class Texture
{
public:
	Texture(std::string filename)
	: filename(filename)
	{
		std::cout << "Texture constructed with: " << filename << std::endl;
	}
private:
	std::string filename;
};

class ShaderProgram
{
public:
	ShaderProgram(std::string filename)
	: filename(filename)
	{
		std::cout << "ShaderProgram constructed with: " << filename << std::endl;
	}
private:
	std::string filename;
};

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
		if(data.find(name) != data.end()) throw 1;
		data.insert(std::make_pair(name, val));
		return val;
	}
	static std::map<std::string, std::shared_ptr<T>> data;
};
template <typename T>
std::map<std::string, std::shared_ptr<T>> Resources<T>::data;

int main()
{
	Resources<ShaderProgram>::Load("hello.vsps");
	Resources<ShaderProgram>::Load("test.vsps");
	Resources<ShaderProgram>::Load("test.vsps");
	Resources<Texture>::Load("hello.png");
	Resources<Texture>::Load("test.png");
	Resources<Texture>::Load("test.png");
	Resources<Texture>::Load("t2est.png", std::shared_ptr<Texture>(new Texture("t2est.png")));
	return 0;
}

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

namespace Obj {

class Material
{
public:
	Material()
	: map_Ka("")
	{
	}
	std::string map_Ka;
};

class MtlParser
{
public:
	MtlParser(std::string filename)
	{
		load(filename);
	}
private:
	void onNewMtl(std::stringstream& line)
	{
		std::string matname;
		line >> matname;
		materials.insert(std::make_pair(matname, Material()));
		curmat = &(materials.find(matname)->second);
	}
	void onmap_Ka(std::stringstream& line)
	{
		std::string map_Ka;
		line >> map_Ka;
		curmat->map_Ka = map_Ka;
	}
	void load(std::string filename)
	{
		std::ifstream file(filename.c_str());
		while(file.good())
		{
			std::string line;
			std::getline(file, line);
			std::stringstream strline(line);
			std::string key;
			strline >> key;
			if(key == "newmtl")
			{
				onNewMtl(strline);
			}
			else if(key == "map_Ka")
			{
				onmap_Ka(strline);
			}
			else if(key.size() == 0)
			{
				// don't handle empty lines
			}
			else if(key.size() > 0 && key[0] == '#')
			{
				// dont't handle comments
			}
			else
			{
				std::cout << "Not handled: " << line << std::endl;
			}
		}
		for( auto i = materials.begin(); i != materials.end(); ++i )
		{
		    std::cout << i->first << ": " << i->second.map_Ka << std::endl;
		}
	}
private:
	Material* curmat;
	std::map<std::string, Material> materials;
};

} // end Obj namespace

int main()
{
	Obj::MtlParser mtl("../data/spike_mtl.mtl");
	return 0;
}

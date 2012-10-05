#include "obj.h"
#include <fstream>
#include "skinnedobject.h"
#include <iostream>

#define FACESIZEISTHREE 3

namespace Obj {
Material::Material()
: map_Ka("")
{
}

MtlParser::MtlParser()
: materials(new MtlMaterials())
{
}
std::unique_ptr<MtlMaterials> MtlParser::load(std::string path, std::string filename)
{
	std::ifstream file((path + "/" + filename).c_str());
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
			//std::cout << "Not handled: " << line << std::endl;
		}
	}
	return move(materials);
}

void MtlParser::onNewMtl(std::stringstream& line)
{
	std::string matname;
	line >> matname;
	materials->insert(std::make_pair(matname, Material()));
	curmat = &(materials->find(matname)->second);
}
void MtlParser::onmap_Ka(std::stringstream& line)
{
	std::string map_Ka;
	line >> map_Ka;
	curmat->map_Ka = map_Ka;
}


ObjException::ObjException(const std::string &errors)
	: errors(errors)
{}
ObjException::~ObjException() throw () {}
const char* ObjException::what() const throw() { return errors.c_str(); }

FaceElement::FaceElement() : v(0), vt(0), vn(0) {}

ObjParser::ObjParser(std::string path, std::string filename)
: path(path)
, materials(new MtlMaterials())
{
	load(path + "/" + filename);
}
Model* ObjParser::GetModel()
{
	for(size_t i = 0; i < faces.size(); i++)
	{
		//TODO: Only supporting faces with three elements for now.
		if(faces[i].faceels.size() != 3) throw ObjException("Only supporting 3 vertex faces");
	}
	size_t vertexcount = faces.size() * 3;
	//TODO: remove this memory leak
	vertex *modelVertices = new vertex[vertexcount];
	for(size_t i = 0; i < faces.size(); i++)
	{
		modelVertices[i*3+0].x = vertices[faces[i].faceels[0].v - 1].x;
		modelVertices[i*3+0].y = vertices[faces[i].faceels[0].v - 1].y;
		modelVertices[i*3+0].z = vertices[faces[i].faceels[0].v - 1].z;
		modelVertices[i*3+1].x = vertices[faces[i].faceels[2].v - 1].x;
		modelVertices[i*3+1].y = vertices[faces[i].faceels[2].v - 1].y;
		modelVertices[i*3+1].z = vertices[faces[i].faceels[2].v - 1].z;
		modelVertices[i*3+2].x = vertices[faces[i].faceels[1].v - 1].x;
		modelVertices[i*3+2].y = vertices[faces[i].faceels[1].v - 1].y;
		modelVertices[i*3+2].z = vertices[faces[i].faceels[1].v - 1].z;
		if(!normals.empty())
		{
			modelVertices[i*3+0].nx = normals[faces[i].faceels[0].vn - 1].x;
			modelVertices[i*3+0].ny = normals[faces[i].faceels[0].vn - 1].y;
			modelVertices[i*3+0].nz = normals[faces[i].faceels[0].vn - 1].z;
			modelVertices[i*3+1].nx = normals[faces[i].faceels[2].vn - 1].x;
			modelVertices[i*3+1].ny = normals[faces[i].faceels[2].vn - 1].y;
			modelVertices[i*3+1].nz = normals[faces[i].faceels[2].vn - 1].z;
			modelVertices[i*3+2].nx = normals[faces[i].faceels[1].vn - 1].x;
			modelVertices[i*3+2].ny = normals[faces[i].faceels[1].vn - 1].y;
			modelVertices[i*3+2].nz = normals[faces[i].faceels[1].vn - 1].z;
		}
		if(!texcoords.empty())
		{
			modelVertices[i*3+0].s0 = texcoords[faces[i].faceels[0].vt - 1].u;
			modelVertices[i*3+0].t0 = texcoords[faces[i].faceels[0].vt - 1].v;
			modelVertices[i*3+1].s0 = texcoords[faces[i].faceels[2].vt - 1].u;
			modelVertices[i*3+1].t0 = texcoords[faces[i].faceels[2].vt - 1].v;
			modelVertices[i*3+2].s0 = texcoords[faces[i].faceels[1].vt - 1].u;
			modelVertices[i*3+2].t0 = texcoords[faces[i].faceels[1].vt - 1].v;
		}
	}
	return new Model(modelVertices, vertexcount, skinnedObjects);
}

void ObjParser::onVertex(std::vector<Vertex>& vertices, std::stringstream& line)
{
	Vertex vertex;
	line >>  vertex.x >> vertex.y >> vertex.z;
	vertices.push_back(vertex);
}

void ObjParser::onTexcoord(std::vector<Texcoord>& texcoords, std::stringstream& line)
{
	Texcoord texcoord;
	line >> texcoord.u >> texcoord.v;
	texcoords.push_back(texcoord);
}

void ObjParser::onFace(std::vector<Face>& faces, std::stringstream& line)
{
	Face face;
	char delim = '/';
	while(line.good())
	{
		FaceElement faceel;
		bool go = false;
		std::string facestr;
		line >> facestr;
		std::stringstream facestream(facestr);

		facestream >> faceel.v;
		if(go || (go = facestream.good()))
		{
			facestream >> delim >> faceel.vt;
		}

		if(go || (go = facestream.good()))
		{
			facestream >> delim >> faceel.vn;
		}

		face.faceels.push_back(faceel);
		if(facestream.good()) std::cout << "Something bad happened" << std::endl;
	}
	faces.push_back(face);
}

void ObjParser::onMtllib(std::stringstream& line)
{
	std::string filename;
	line >> filename;
	MtlParser parser;
	materials = parser.load(path, filename);
}
void ObjParser::onUsemtl(std::stringstream& line)
{
	std::string materialname;
	line >> materialname;
	auto iter = materials->find(materialname);
	SkinnedObject skinnedObject;
	if(iter != materials->end())
	{
		skinnedObject.imagename = path + "/" + iter->second.map_Ka;
	}
	else
	{
		skinnedObject.imagename = "data/gold.png";
	}
	skinnedObject.offset = faces.size() * FACESIZEISTHREE;
	skinnedObjects.push_back(skinnedObject);
}
void ObjParser::SetSizesSkinnedObjects()
{
	size_t size = skinnedObjects.size();
	for(size_t i = 0; i < size - 1; ++i)
	{
		skinnedObjects[i].size = skinnedObjects[i + 1].offset - skinnedObjects[i].offset;
	}
	if(skinnedObjects.size() > 0)
	{
		skinnedObjects[size - 1].size = (faces.size() * FACESIZEISTHREE) - skinnedObjects[size - 1].offset;
	}
}
void ObjParser::load(std::string filename)
{
	std::ifstream file(filename.c_str());
	while(file.good())
	{
		std::string line;
		std::getline(file, line);
		std::stringstream strline(line);
		std::string key;
		strline >> key;
		if(key == "v")
		{
			onVertex(vertices, strline);
		}
		else if(key == "vn")
		{
			onVertex(normals, strline);
		}
		else if(key == "vt")
		{
			onTexcoord(texcoords, strline);
		}
		else if(key == "f")
		{
			onFace(faces, strline);
		}
		else if(key == "mtllib")
		{
			onMtllib(strline);
		}
		else if(key == "usemtl")
		{
			onUsemtl(strline);
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
			//std::cout << "Not handled: " << line << std::endl;
		}
	}
	SetSizesSkinnedObjects();
}
} // end Obj namespace


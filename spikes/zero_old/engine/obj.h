#ifndef OBJ_H_
#define OBJ_H_

#include <memory>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>  
#include "model.h"

namespace Obj {
		
class Material
{
public:
	Material();
	std::string map_Ka;
};

typedef std::map<std::string, Material> MtlMaterials;

class MtlParser
{
public:
	MtlParser();
	std::unique_ptr<MtlMaterials> load(std::string path, std::string filename);
private:
	void onNewMtl(std::stringstream& line);
	void onmap_Ka(std::stringstream& line);
	std::unique_ptr<MtlMaterials> materials;
	Material* curmat;
};


class ObjException : public std::exception
{
public:
	ObjException(const std::string &errors);
	~ObjException() throw ();
	const char *what() const throw();
private:
	const std::string errors;
};

typedef glm::vec3 Vertex;

struct FaceElement
{
	FaceElement();
	short v, vt, vn;
};

struct Face
{
	std::vector<FaceElement> faceels;
};

struct Texcoord
{
	float u, v;
};

class ObjParser
{
public:
	ObjParser(std::string path, std::string filename);
	Model* GetModel();
private:
	void onVertex(std::vector<Vertex>& vertices, std::stringstream& line);
	void onTexcoord(std::vector<Texcoord>& texcoords, std::stringstream& line);
	void onFace(std::vector<Face>& faces, std::stringstream& line);
	void onMtllib(std::stringstream& line);
	void onUsemtl(std::stringstream& line);
	void SetSizesSkinnedObjects();
	void load(std::string filename);
private:
	std::string path;
	std::unique_ptr<MtlMaterials> materials;
	std::vector<Vertex> vertices;
	std::vector<Vertex> normals;
	std::vector<Texcoord> texcoords;
	std::vector<Face> faces;
	std::vector<SkinnedObject> skinnedObjects;
};
} // end Obj namespace

#endif


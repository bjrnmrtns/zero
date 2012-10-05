#ifndef MODEL_H_
#define MODEL_H_

#include <vector>
#include <map>
#include "skinnedobject.h"
#include "vertex.h"
#include "shaderprogram.h"
#include <GL/glew.h>
#include "vertexbuffer.h"
#include <memory>
#include "texture.h"

static VertexBuffer::AttributeReference modelRefs[] =
{
	{ 0, 3  },
	{ 12, 3 },
	{ 24, 2 },
	{ 0, 0  }
};

class Model : public Renderable
{
public:
	Model(vertex *vertices, int vertexcount);
	Model(vertex *vertices, int vertexcount, std::vector<SkinnedObject> skinnedObjects);
	~Model();
	void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES);
	static Model& Square();
	static Model& Cube();
private:
	std::map<std::string, std::shared_ptr<Texture>> textures;
	VertexBuffer vertexBuffer;
	std::vector<SkinnedObject> skinnedObjects;
};

#endif


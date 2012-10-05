#include "model.h"
#include <iostream>
#include "resources.h"

Model::Model(vertex *vertices, int vertexcount)
: vertexBuffer(modelRefs, vertices, vertexcount)
{
}

Model::Model(vertex *vertices, int vertexcount, std::vector<SkinnedObject> skinnedObjects)
: vertexBuffer(modelRefs, vertices, vertexcount)
, skinnedObjects(skinnedObjects)
{
	for(auto iter = skinnedObjects.begin(); iter != skinnedObjects.end(); ++iter)
	{
		if(textures.find(iter->imagename) == textures.end())
		{
			std::cout << "Load texture: " << iter->imagename << std::endl;
			textures.insert(std::make_pair(iter->imagename, Resources<Texture>::Load(iter->imagename)));
		}
	}
}

Model::~Model()
{
}

void Model::Draw(ShaderProgram& shaderProgram, unsigned int type)
{
	if(skinnedObjects.size() == 0)
	{
		if(textures.size() > 0)
		{
			Texture::BlockTexture().Bind(shaderProgram, "modeltex");
		}
		vertexBuffer.Draw(shaderProgram, type);
	}
	else
	{
		for(auto skinnedObject = skinnedObjects.begin(); skinnedObject != skinnedObjects.end(); ++skinnedObject)
		{
			auto texture = textures.find(skinnedObject->imagename);
			texture->second->Bind(shaderProgram, "modeltex");
			vertexBuffer.Draw(shaderProgram, skinnedObject->offset, skinnedObject->size);
		}
	}
}
Model& Model::Square()
{
	static vertex v[] = {
		{ -10, 0, -10,  0,  0, 1,  0,  0 },
		{  10, 0, -10,  0,  0, 1,  1,  0 },
		{ -10, 0,  10,  0,  0, 1,  0,  1 },

		{ -10, 0,  10,  0,  0, -1,  0,  1 },
		{  10, 0, -10,  0,  0, -1,  1,  0 },
		{  10, 0,  10,  0,  0, -1,  1,  1 },

	};
	static Model square(v, sizeof(v)/sizeof(vertex));
	return square;
}
Model& Model::Cube()
{
	static vertex v[] = {
		{ -1,  1, -1, -1,  0,  0,  0,  0 },
		{ -1, -1,  1, -1,  0,  0,  1,  1 },
		{ -1, -1, -1, -1,  0,  0,  1,  0 },

		{ -1, -1,  1, -1,  0,  0,  1,  1 },
		{ -1,  1, -1, -1,  0,  0,  0,  0 },
		{ -1,  1,  1, -1,  0,  0,  0,  1 },

		{ -1,  1,  1,  0,  0,  1,  0,  0 },
		{  1, -1,  1,  0,  0,  1,  1,  1 },
		{ -1, -1,  1,  0,  0,  1,  1,  0 },
		
		{  1, -1,  1,  0,  0,  1,  1,  1 },
		{ -1,  1,  1,  0,  0,  1,  0,  0 },
		{  1,  1,  1,  0,  0,  1,  0,  1 },

		{  1, -1,  1,  1,  0,  0,  0,  0 },
		{  1,  1, -1,  1,  0,  0,  1,  1 },
		{  1, -1, -1,  1,  0,  0,  1,  0 },

		{  1,  1, -1,  1,  0,  0,  1,  1 },
		{  1, -1,  1,  1,  0,  0,  0,  0 },
		{  1,  1,  1,  1,  0,  0,  0,  1 },

		{  1,  1, -1,  0,  0, -1,  0,  0 },
		{ -1, -1, -1,  0,  0, -1,  1,  1 },
		{  1, -1, -1,  0,  0, -1,  0,  1 },

		{ -1, -1, -1,  0,  0, -1,  1,  1 },
		{  1,  1, -1,  0,  0, -1,  0,  0 },
		{ -1,  1, -1,  0,  0, -1,  1,  0 },

		{ -1,  1, -1,  0,  1,  0,  0,  0 },
		{  1,  1,  1,  0,  1,  0,  1,  1 },
		{ -1,  1,  1,  0,  1,  0,  1,  0 },

		{  1,  1,  1,  0,  1,  0,  1,  1 },
		{ -1,  1, -1,  0,  1,  0,  0,  0 },
		{  1,  1, -1,  0,  1,  0,  0,  1 },

		{  1, -1, -1,  0, -1,  0,  0,  0 },
		{ -1, -1,  1,  0, -1,  0,  1,  1 },
		{  1, -1,  1,  0, -1,  0,  1,  0 },

		{ -1, -1,  1,  0, -1,  0,  1,  1 },
		{  1, -1, -1,  0, -1,  0,  0,  0 },
		{ -1, -1, -1,  0, -1,  0,  0,  1 }
	};

	static Model cube(v, sizeof(v)/sizeof(vertex));
	return cube;
}

#ifndef UI__
#define UI__

#include <Rocket/Core/FileInterface.h>
#include <Rocket/Core/RenderInterface.h>
#include <Rocket/Core/SystemInterface.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <map>
#include <memory>
#include "ShaderProgram.hpp"
#include "InputElementDescription.hpp"

class UISystemInterface : public Rocket::Core::SystemInterface
{
private:
	sf::Clock clock;
public:
	float GetElapsedTime()
	{
		return clock.getElapsedTime().asSeconds();
	}
};

class UIFileInterface : public Rocket::Core::FileInterface
{
public:
	const Rocket::Core::String root;
	UIFileInterface(const Rocket::Core::String root)
	: root(root)
	{
	}
	Rocket::Core::FileHandle Open(const Rocket::Core::String& path)
	{
		FILE* fp = fopen((root + path).CString(), "rb");
		if (fp != NULL) return (Rocket::Core::FileHandle) fp;
		throw GeneralException((root + path).CString());
	}
	void Close(Rocket::Core::FileHandle file)
	{
		fclose((FILE*) file);
	}
	size_t Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
	{
		return fread(buffer, 1, size, (FILE*) file);
	}
	bool Seek(Rocket::Core::FileHandle file, long offset, int origin)
	{
		return fseek((FILE*) file, offset, origin) == 0;
	}
	size_t Tell(Rocket::Core::FileHandle file)
	{
		return ftell((FILE*) file);
	}
};

const InputElementDescription uidescription[] { { "in_position", 2, sizeof(glm::vec2), GL_FLOAT },
                                              { "in_color",    4, sizeof(glm::vec4), GL_UNSIGNED_BYTE },
                                              { "in_texcoord",    2, sizeof(glm::vec2), GL_FLOAT },
                                              { "", 0, 0, 0 } };
class UIRenderer : public Rocket::Core::RenderInterface
{
private:
	std::map<Rocket::Core::TextureHandle, sf::Texture*> textures;
	ShaderProgram sp;
public:
	UIRenderer()
	: sp("ui.vs", "ui.fs", uidescription)
	{
	}
	void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
	{
	}
	bool LoadTexture(Rocket::Core::TextureHandle& texture_handle,
	                 Rocket::Core::Vector2i& texture_dimensions,
	                 const Rocket::Core::String& source)
	{
		sf::Texture* texture = new sf::Texture();
		bool ret = texture->loadFromFile(source.CString());
		textures.insert(std::make_pair(texture_handle, texture));
		return ret;
	}

	bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
	                     const Rocket::Core::byte* source,
	                     const Rocket::Core::Vector2i& source_dimensions)
	{
		sf::Texture* texture = new sf::Texture();
		bool ret = texture->loadFromMemory(source, source_dimensions.x * source_dimensions.y * 4);
		textures.insert(std::make_pair(texture_handle, texture));
		return ret;
	}
	void ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
	{
		auto it = textures.find(texture_handle);
		if(it != textures.end())
		{
			delete it->second;
			textures.erase(it);
		}
	}
	~UIRenderer()
	{
		for(auto it = textures.begin(); it != textures.end(); ++it)
		{
			delete it->second;
		}
	}

};

#endif

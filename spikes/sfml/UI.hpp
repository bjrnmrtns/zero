#ifndef UI__
#define UI__

#include <Rocket/Core/FileInterface.h>
#include <Rocket/Core/RenderInterface.h>
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <map>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderProgram.hpp"
#include "InputElementDescription.hpp"
#include "VertexBuffer.hpp"
#include <iostream>

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
		FILE* fp = fopen((root + "/" + path).CString(), "rb");
		if (fp != NULL) return (Rocket::Core::FileHandle) fp;
		throw GeneralException((root + "/"  + path).CString());
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

const InputElementDescription uidescription[] { { "in_position", 2, sizeof(glm::vec2), GL_FLOAT, false },
                                              { "in_color", 4, 4 * sizeof(unsigned char), GL_UNSIGNED_BYTE, true },
                                              { "in_texcoord", 2, sizeof(glm::vec2), GL_FLOAT, false },
                                              { "", 0, 0, 0, false } };
class UIRenderer : public Rocket::Core::RenderInterface
{
private:
	std::map<Rocket::Core::TextureHandle, sf::Texture*> textures;
	ShaderProgram sp;
public:
	UIRenderer()
	: sp("ui.vs", "ui.fs", uidescription)
	{
		sp.Use();
		sp.SetTexture("uitexture", 0);
	}
	void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
	{
		sp.Set("projection", &glm::ortho<float>(0.0f, 800, 600, 0, -1.0f, 1.0f)[0][0]);
		glm::mat4 trans= glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, 0.0f));
		sp.Set("model", &trans[0][0]);
		VertexBuffer vb(uidescription, vertices, num_vertices);
		glActiveTexture(GL_TEXTURE0);
		textures.find(texture)->second->bind();
		vb.DrawIndexed(*indices, num_indices);
	}
	bool LoadTexture(Rocket::Core::TextureHandle& texture_handle,
	                 Rocket::Core::Vector2i& texture_dimensions,
	                 const Rocket::Core::String& source)
	{
		sf::Texture* texture = new sf::Texture();
		Rocket::Core::String img("ui/" + source);
		bool ret = texture->loadFromFile(img.CString());
		texture_dimensions.x = texture->getSize().x;
		texture_dimensions.y = texture->getSize().y;
		textures.insert(std::make_pair(texture_handle, texture));
		return ret;
	}

	bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
	                     const Rocket::Core::byte* source,
	                     const Rocket::Core::Vector2i& source_dimensions)
	{
		sf::Texture* texture = new sf::Texture();
		bool ret = texture->create(source_dimensions.x, source_dimensions.y);
		texture->update(source);
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
	void EnableScissorRegion(bool enable)
	{
		if(enable)
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}
	}
	void SetScissorRegion(int x, int y, int width, int height)
	{
		glScissor(x, y, width, height);
	}
	~UIRenderer()
	{
		for(auto it = textures.begin(); it != textures.end(); ++it)
		{
			delete it->second;
		}
	}

};

class RocketUI
{
private:
	UIRenderer renderer;
	UIFileInterface fileinterface;
	UISystemInterface systeminterface;
	Rocket::Core::Context* Context;
	Rocket::Core::ElementDocument *Document;
public:
	RocketUI(size_t width, size_t height)
	: fileinterface("ui")
	{
		Rocket::Core::SetSystemInterface(&systeminterface);
		Rocket::Core::SetFileInterface(&fileinterface);
		Rocket::Core::SetRenderInterface(&renderer);
		if(!Rocket::Core::Initialise())
			throw new std::exception();

		Rocket::Core::FontDatabase::LoadFontFace("PressStart2P.ttf");

		Context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(width, height));
		Document = Context->LoadDocument("zero.rml");
		Document->Show();
		Document->RemoveReference();
	}
	void UpdateAndRender()
	{
		Context->Update();
		Context->Render();
	}
	void setFPS(std::string fps)
	{
		Document->GetElementById("fps")->SetInnerRML(fps.c_str());
	}
	void setConsole(std::string console)
	{
		Document->GetElementById("console")->SetInnerRML(console.c_str());
	}
};


#endif

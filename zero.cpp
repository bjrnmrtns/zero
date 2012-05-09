#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <exception>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <IL/il.h>
#include <string.h>
#include <cstdlib>
#include <map>
#include "Blob.cpp"
#include "GeneralException.cpp"
#include "md5.cpp"

class timer
{
private:
	double start, last;
public:
	timer()
	: start(glfwGetTime()) // high precision timer (time since glfwInit()).
	, last(start)
	{
	}
	double sincelast()
	{
		double now = glfwGetTime();
		double retval = now - last;
		last = now;
		return retval;
	}
};

class File
{
public:
	FILE* file;
	File(const std::string name, const char* mode)
	: file(fopen(name.c_str(), mode))
	{
		if(file == 0) throw GeneralException("could not open file");
	}
	~File()
	{
		fclose(file);
	}
	static Blob read(const std::string name)
	{
		File file(name, "r");
		fseek(file.file, 0L, SEEK_END);
		Blob blob;
		blob.size = ftell(file.file);
		rewind(file.file);
		unsigned char* buf = (unsigned char*)malloc(blob.size);
		blob.buf.reset(buf);
		size_t done = 0;
		do {
			size_t read = fread(buf+done, 1, blob.size - done, file.file);
			assert(read != 0);
			done += read;
		} while(done != blob.size);
		return blob;
	}
	static void write(const std::string name, const Blob& blob)
	{
		File file(name, "w");
		size_t done = 0;
		do {
			size_t written = fwrite(blob.buf.get()+done, 1, blob.size - done, file.file);
			assert(written != 0);
			done += written;
		} while(done != blob.size);
	}
};

template <typename T>
class Res
{
public:
	static T* load(std::string name)
	{
		auto it = data.find(name);
		if(it != data.end()) return it->second.get();
		assert(false);

	}
	static T* load(std::string name, T* val)
	{
		if(data.find(name) != data.end()) throw GeneralException("Resource: " + name + ", already loaded");
		data.insert(std::make_pair(name, std::unique_ptr<T>(val)));
		return val;
	}
	static std::map<std::string, std::unique_ptr<T>> data;
};
template <typename T>
std::map<std::string, std::unique_ptr<T>> Res<T>::data;

class VertexBuffer
{
public:
	struct InputElementDescription
	{
		std::string name;
		size_t numberofelements;
		size_t elementsize;
	};
	template <typename T>
	VertexBuffer(const InputElementDescription description[], const T vertexData[], size_t count, unsigned int mode = GL_TRIANGLES)
	: stride(sizeof(T))
	, count(count)
	, mode(mode)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		size_t offset = 0;
		for(int i = 0; description[i].elementsize; ++i)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, description[i].numberofelements, GL_FLOAT, GL_FALSE, stride, (void *)offset);
			offset += description[i].elementsize;
		}
//		glBufferData(GL_ARRAY_BUFFER, stride * count, vertexData, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, stride * count, vertexData, GL_DYNAMIC_DRAW);
	}
	template <typename T>
	void Update(const T vertexData[], size_t count)
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * count, vertexData, GL_DYNAMIC_DRAW);
	}
	void Draw() const
	{
		glBindVertexArray(vao);
		glDrawArrays(mode, 0, count);
	}
	void DrawIndexed(int& indices, int num_indices) const
	{
		glBindVertexArray(vao);
		glDrawElements(mode, num_indices, GL_UNSIGNED_INT, &indices);
	}
	~VertexBuffer()
	{
		glDeleteBuffers(1, &id);
		glDeleteVertexArrays(1, &vao);
	}
private:
	unsigned int id;
	unsigned int vao;
	unsigned int stride;
	size_t count;
	unsigned int mode;
};

class Shader
{
public:
	Shader(const std::string filename, int type) : type(type)
	{
		Blob shaderdata = File::read(filename);
		const char * source = (const char*)shaderdata.buf.get();
		id = glCreateShader(type);
		glShaderSource(id, 1, &source, (int *)&shaderdata.size);
		glCompileShader(id);
		int ok = true;
		glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			int length = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char> errors(new char[length]);
			glGetShaderInfoLog(id, length, 0, errors.get());
			throw GeneralException(std::string(errors.get()));
		}
	}
	virtual ~Shader()
	{
		glDeleteShader(id);
	}
	unsigned int GetId() { return id; }
private:
	unsigned int id;
	int type;
};

class VertexShader : public Shader
{
public:
	VertexShader(const std::string filename) : Shader(filename, GL_VERTEX_SHADER) {}
};

class FragmentShader : public Shader
{
public:
	FragmentShader(const std::string filename) : Shader(filename, GL_FRAGMENT_SHADER) {}
};

class ShaderProgram
{
public:
	ShaderProgram(VertexShader &vertexShader, FragmentShader &fragmentShader, const VertexBuffer::InputElementDescription description[])
	: vertexShader(vertexShader)
	, fragmentShader(fragmentShader)
	{
		id = glCreateProgram();
		if(id == 0) throw GeneralException("glCreateProgram of shader failed");
		glAttachShader(id, vertexShader.GetId());
		glAttachShader(id, fragmentShader.GetId());

		for(int i = 0; description[i].elementsize; ++i)
		{
			glBindAttribLocation(id, i, description[i].name.c_str());
		}
		glLinkProgram(id);

		int ok = true;
		glGetShaderiv(id, GL_LINK_STATUS, &ok);
		glGetError(); //TODO: remove this because it is used to ignore the error GL_INVALID_OPERATION after glGetShaderiv
		if(!ok)
		{
			int length = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char> errors(new char[length]);
			glGetProgramInfoLog(id, length, 0, errors.get());
			throw GeneralException(std::string(errors.get()));
		}
	}
	~ShaderProgram()
	{
		glDetachShader(id, fragmentShader.GetId());
		glDetachShader(id, vertexShader.GetId());
		glDeleteProgram(id);
	}
	void Set(const char *name, const float mat[16])
	{
		int uniform = glGetUniformLocation(id, name);
		if (uniform == -1) return;
		glUseProgram(id);
		glUniformMatrix4fv(uniform, 1, GL_FALSE, mat);
		int error = glGetError(); if (error) { printf("%d\n", error); abort(); }
	}
	bool SetTexture(const char *name, int value)
	{
		int error;
		int uniform = glGetUniformLocation(id, name);
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		if (uniform == -1) return false;
		glUseProgram(id);
		glUniform1i(uniform, value);
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		return true;
	}
	void Use()
	{
		glUseProgram(id);
	}
private:
	unsigned int id;
	VertexShader& vertexShader;
	FragmentShader& fragmentShader;
};

class Texture
{
	friend class Image;
public:
	struct ImageData
	{
		int type;
		Blob blob;
	};
	class Image
	{
	private:
		Texture& texture;
		unsigned int id;
	public: 
		Image(Texture& texture)
		: texture(texture)
		{
			ilGenImages(1, &id);
			ilBindImage(id);
			ilTexImage(texture.width, texture.height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, 0);
		}
		void load(ImageData& imagedata)
		{
			ilBindImage(id);
			texture.Bind(0);
			ilLoadL(imagedata.type, imagedata.blob.buf.get(), imagedata.blob.size);
			assert(ilGetData() != 0);
			texture.width = ilGetInteger(IL_IMAGE_WIDTH);
			texture.height = ilGetInteger(IL_IMAGE_HEIGHT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
		}
		ImageData save()
		{
			ilBindImage(id);
			std::unique_ptr<unsigned char> data = texture.getdata();
			ilSetData(data.get());
			ImageData imagedata;
			imagedata.type = IL_PNG;
			imagedata.blob.size = ilSaveL(IL_PNG, data.get(), texture.width * texture.height * 3);
			unsigned char* savedata = (unsigned char*)malloc(imagedata.blob.size);
			imagedata.blob.buf.reset(savedata);
			memcpy(savedata, data.get(), imagedata.blob.size);
			return imagedata;
		}
		~Image()
		{
			ilDeleteImages(1, &id);
		}
	};
private:
	unsigned int width;
	unsigned int height;
	Image image;
	unsigned int id;
public:
	Texture(unsigned int width, unsigned int height)
	: width(width)
	, height(height)
	, image(*this)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	}
	Texture(unsigned int width, unsigned int height, ImageData& imagedata)
	: width(width)
	, height(height)
	, image(*this)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		load(imagedata);
	}
	Texture(unsigned int width, unsigned int height, unsigned char* data)
	: width(width)
	, height(height)
	, image(*this)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, data);
	}
	// Bind for render onto primitive.
	void Bind(int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0+textureUnit);
		glBindTexture(GL_TEXTURE_2D, id);
        }
	// Attach for render to texture using fbo
	void Attach(unsigned int nr)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + nr, id, 0);
	}
	std::unique_ptr<unsigned char> getdata()
	{
		std::unique_ptr<unsigned char> data(new unsigned char[width * height * 3]);
		Bind(0);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, data.get());
		return data;
	}
	void load(ImageData& imagedata)
	{
		image.load(imagedata);
	}
	ImageData save()
	{
		return image.save();
	}
	~Texture()
	{
		glDeleteTextures(1, &id);
	}
};

static const unsigned int Attachments[] =
{
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10,
	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14,
	GL_COLOR_ATTACHMENT15
};

class RenderTarget
{
public:
	RenderTarget(unsigned int width, unsigned int height, std::vector<Texture*>& targets)
	: width(width)
	, height(height)
	, fbo(0)
	, rbo(0)
	{
		if(targets.size() > 0)
		{
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
			glDrawBuffers(targets.size(), Attachments);
			for(unsigned int i=0; i < targets.size(); i++)
			{
				targets[i]->Attach(i);
			}
		}
	}
	void Activate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	~RenderTarget()
	{
		if(fbo != 0)
		{
			glDeleteRenderbuffers(1, &rbo);
			glDeleteFramebuffers(1, &fbo);
		}
	}
private:
	unsigned int width, height;
	unsigned int fbo, rbo;
};

class Window_
{
public:
	Window_(size_t width, size_t height)
	{
		ilInit();
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		ilEnable(IL_ORIGIN_SET);
		if(!glfwInit()) throw new GeneralException("glfwInit failed");
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		if(!glfwOpenWindow(width, height, 8, 8, 8, 0, 24, 8, GLFW_WINDOW))
		{
			glfwTerminate();
			throw new GeneralException("glfwOpenWindow failed");
		}
		glewExperimental = GL_TRUE;
		if(glewInit() != GLEW_OK) throw new GeneralException("glewInit failed");
		glGetError(); // mask error of failed glewInit http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=284912
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glViewport(0, 0, width, height);
	}
	void Swap()
	{
		glfwSwapBuffers();
	}
	~Window_()
	{
		glfwTerminate();
	}
};

class object
{
public:
	virtual void Draw(ShaderProgram& sp) const = 0;
};

class mesh
{
public:
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};
private:
	VertexBuffer vb;
public:
	static const VertexBuffer::InputElementDescription description[];
	mesh(const VertexBuffer::InputElementDescription description[], const Vertex vertices[], const size_t size, unsigned int mode = GL_TRIANGLES)
	: vb(description, vertices, size, mode)
	{
	}
	static mesh& heightmap()
	{
		const size_t size = 128;
		const size_t spacing = 1.0f;
		static Vertex vertices[size * size * 6];
		for(size_t x = 0; x < size; x++)
		{
			for(size_t z = 0; z < size; z++)
			{
				int xw =  (2 * x * spacing) - (size * spacing);
				int zw =  (2 * z * spacing) - (size * spacing);
				vertices[6 * ((z * size) + x) + 0].pos = glm::vec3(xw - spacing, 0, zw - spacing);
				vertices[6 * ((z * size) + x) + 0].normal = glm::vec3(0, 1, 0);
				vertices[6 * ((z * size) + x) + 0].texcoord = glm::vec2(0, 0);
				vertices[6 * ((z * size) + x) + 1].pos = glm::vec3(xw - spacing, 0, zw + spacing);
				vertices[6 * ((z * size) + x) + 1].normal = glm::vec3(0, 1, 0);
				vertices[6 * ((z * size) + x) + 1].texcoord = glm::vec2(-1, -1);
				vertices[6 * ((z * size) + x) + 2].pos = glm::vec3(xw + spacing, 0, zw - spacing);
				vertices[6 * ((z * size) + x) + 2].normal = glm::vec3(0, 1, 0);
				vertices[6 * ((z * size) + x) + 2].texcoord = glm::vec2(-1, -1);
				vertices[6 * ((z * size) + x) + 3].pos = glm::vec3(xw - spacing, 0, zw + spacing);
				vertices[6 * ((z * size) + x) + 3].normal = glm::vec3(0, 1, 0);
				vertices[6 * ((z * size) + x) + 3].texcoord = glm::vec2(-1, -1);
				vertices[6 * ((z * size) + x) + 4].pos = glm::vec3(xw + spacing, 0, zw + spacing);
				vertices[6 * ((z * size) + x) + 4].normal = glm::vec3(0, 1, 0);
				vertices[6 * ((z * size) + x) + 4].texcoord = glm::vec2(-1, -1);
				vertices[6 * ((z * size) + x) + 5].pos = glm::vec3(xw + spacing, 0, zw - spacing);
				vertices[6 * ((z * size) + x) + 5].normal = glm::vec3(0, 1, 0);
				vertices[6 * ((z * size) + x) + 5].texcoord = glm::vec2(-1, -1);
			}
		}
		static mesh model(description, vertices, sizeof(vertices)/sizeof(Vertex));
		return model;
	};
	static mesh& square()
	{
		static const Vertex vertices[] { { glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
		                                 { glm::vec3(-1,  1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) },
		                                 { glm::vec3( 1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
		                                 { glm::vec3(-1,  1, 1), glm::vec3(0, 0,-1), glm::vec2(0, 1) },
		                                 { glm::vec3( 1,  1, 1), glm::vec3(0, 0,-1), glm::vec2(1, 1) },
		                                 { glm::vec3( 1, -1, 1), glm::vec3(0, 0,-1), glm::vec2(1, 0) } };
		static mesh model(description, vertices, sizeof(vertices)/sizeof(Vertex));
		return model;
	};
	static mesh& cube()
	{
	static Vertex vertices[] = { { glm::vec3(-1,  1, -1), glm::vec3(-1,  0,  0),  glm::vec2(0,  0) },
	                             { glm::vec3(-1, -1,  1), glm::vec3(-1,  0,  0),  glm::vec2(1,  1) },
	                             { glm::vec3(-1, -1, -1), glm::vec3(-1,  0,  0),  glm::vec2(1,  0) }, 

	                             { glm::vec3(-1, -1,  1), glm::vec3(-1,  0,  0),  glm::vec2(1,  1) },
	                             { glm::vec3(-1,  1, -1), glm::vec3(-1,  0,  0),  glm::vec2(0,  0) },
	                             { glm::vec3(-1,  1,  1), glm::vec3(-1,  0,  0),  glm::vec2(0,  1) },

	                             { glm::vec3(-1,  1,  1), glm::vec3( 0,  0,  1),  glm::vec2(0,  0) },
	                             { glm::vec3( 1, -1,  1), glm::vec3( 0,  0,  1),  glm::vec2(1,  1) },
	                             { glm::vec3(-1, -1,  1), glm::vec3( 0,  0,  1),  glm::vec2(1,  0) },

	                             { glm::vec3( 1, -1,  1), glm::vec3( 0,  0,  1),  glm::vec2(1,  1) },
	                             { glm::vec3(-1,  1,  1), glm::vec3( 0,  0,  1),  glm::vec2(0,  0) },
	                             { glm::vec3( 1,  1,  1), glm::vec3( 0,  0,  1),  glm::vec2(0,  1) },

	                             { glm::vec3( 1, -1,  1), glm::vec3( 1,  0,  0),  glm::vec2(0,  0) },
	                             { glm::vec3( 1,  1, -1), glm::vec3( 1,  0,  0),  glm::vec2(1,  1) },
	                             { glm::vec3( 1, -1, -1), glm::vec3( 1,  0,  0),  glm::vec2(1,  0) },

	                             { glm::vec3( 1,  1, -1), glm::vec3( 1,  0,  0),  glm::vec2(1,  1) },
	                             { glm::vec3( 1, -1,  1), glm::vec3( 1,  0,  0),  glm::vec2(0,  0) },
	                             { glm::vec3( 1,  1,  1), glm::vec3( 1,  0,  0),  glm::vec2(0,  1) },

	                             { glm::vec3( 1,  1, -1), glm::vec3( 0,  0, -1),  glm::vec2(0,  0) },
	                             { glm::vec3(-1, -1, -1), glm::vec3( 0,  0, -1),  glm::vec2(1,  1) },
	                             { glm::vec3( 1, -1, -1), glm::vec3( 0,  0, -1),  glm::vec2(0,  1) },

	                             { glm::vec3(-1, -1, -1), glm::vec3( 0,  0, -1),  glm::vec2(1,  1) },
	                             { glm::vec3( 1,  1, -1), glm::vec3( 0,  0, -1),  glm::vec2(0,  0) },
	                             { glm::vec3(-1,  1, -1), glm::vec3( 0,  0, -1),  glm::vec2(1,  0) },

	                             { glm::vec3(-1,  1, -1), glm::vec3( 0,  1,  0),  glm::vec2(0,  0) },
	                             { glm::vec3( 1,  1,  1), glm::vec3( 0,  1,  0),  glm::vec2(1,  1) },
	                             { glm::vec3(-1,  1,  1), glm::vec3( 0,  1,  0),  glm::vec2(1,  0) },

	                             { glm::vec3( 1,  1,  1), glm::vec3( 0,  1,  0),  glm::vec2(1,  1) },
	                             { glm::vec3(-1,  1, -1), glm::vec3( 0,  1,  0),  glm::vec2(0,  0) },
	                             { glm::vec3( 1,  1, -1), glm::vec3( 0,  1,  0),  glm::vec2(0,  1) },

	                             { glm::vec3( 1, -1, -1), glm::vec3( 0, -1,  0),  glm::vec2(0,  0) },
	                             { glm::vec3(-1, -1,  1), glm::vec3( 0, -1,  0),  glm::vec2(1,  1) },
	                             { glm::vec3( 1, -1,  1), glm::vec3( 0, -1,  0),  glm::vec2(1,  0) },

	                             { glm::vec3(-1, -1,  1), glm::vec3( 0, -1,  0),  glm::vec2(1,  1) },
	                             { glm::vec3( 1, -1, -1), glm::vec3( 0, -1,  0),  glm::vec2(0,  0) },
	                             { glm::vec3(-1, -1, -1), glm::vec3( 0, -1,  0),  glm::vec2(0,  1) }
		};
		static mesh model(description, vertices, sizeof(vertices)/sizeof(Vertex));
		return model;
	}
	static void animfrommd5(std::string animfile, md5::anim& a)
	{
		Tokenizer tok(animfile, md5::whitespace, md5::delimiters);
		md5::animfile::parse(tok, a);
	}
	void nextmd5frame(const md5::model& m, md5::anim& a, std::vector<mesh::Vertex>& vertices, size_t framenr)
	{
		const std::vector<md5::baseframeval>& baseframe = a.baseframe;
		const md5::frame& frame = a.frames[framenr];
		std::vector<md5::joint> joints;
		for(size_t i = 0; i < a.joints.size(); i++)
		{
			md5::joint& j = a.joints[i];
			//TODO: can also be done when loading so joints start with correct base position.
			j.pos = baseframe[i].pos;
			j.orient = baseframe[i].orient;
			unsigned int k = 0;
			if(j.flags & 1 && j.parentid != -1)
			{
				j.pos.x = frame.diffs[j.index + k++];
			}
			if(j.flags & 2 && j.parentid != -1)
			{
				j.pos.z = -frame.diffs[j.index + k++];
			}
			if(j.flags & 4 && j.parentid != -1)
			{
				j.pos.y = frame.diffs[j.index + k++];
			}
			if(j.flags & 8)
			{
				j.orient.x = frame.diffs[j.index + k++];
			}
			if(j.flags & 16)
			{
				j.orient.z = -frame.diffs[j.index + k++];
			}
			if(j.flags & 32)
			{
				j.orient.y = frame.diffs[j.index + k++];
			}
			md5::calcwcomp(j.orient);
			if(j.parentid >= 0)
			{
				md5::joint& pj = a.joints[j.parentid];
				glm::vec3 rpos = pj.orient * j.pos;
				j.pos = rpos + pj.pos;
				j.orient = glm::normalize(pj.orient * j.orient);
			}
			joints.push_back(j);
		}
		//TODO: remove dirty copy past from meshesfrommd5 function.
		size_t vi = 0;
		size_t vi2 = 0;
		for(auto meshit = m.meshes.begin(); meshit != m.meshes.end(); ++meshit)
		{
			std::map<unsigned int, glm::vec3> normals;

			for(auto trisit = meshit->triangles.begin(); trisit != meshit->triangles.end(); ++trisit)
			{

				mesh::Vertex v0;
				v0.pos = md5::meshfile::getfinalpos(meshit->vertices[trisit->v0], *meshit, joints);
				v0.texcoord = meshit->vertices[trisit->v0].texcoord;
				mesh::Vertex v1;
				v1.pos = md5::meshfile::getfinalpos(meshit->vertices[trisit->v1], *meshit, joints);
				v1.texcoord = meshit->vertices[trisit->v1].texcoord;
				mesh::Vertex v2;
				v2.pos = md5::meshfile::getfinalpos(meshit->vertices[trisit->v2], *meshit, joints);
				v2.texcoord = meshit->vertices[trisit->v2].texcoord;
				glm::vec3 normal = glm::cross(v2.pos - v0.pos, v1.pos - v0.pos);
				normals[trisit->v0] += normal;
				normals[trisit->v1] += normal;
				normals[trisit->v2] += normal;
				vertices[vi++] = v0;
				vertices[vi++] = v1;
				vertices[vi++] = v2;
			} 
			for(auto trisit = meshit->triangles.begin(); trisit != meshit->triangles.end(); ++trisit)
			{
				vertices[vi2++].normal = glm::normalize(normals[trisit->v0]);
				vertices[vi2++].normal = glm::normalize(normals[trisit->v1]);
				vertices[vi2++].normal = glm::normalize(normals[trisit->v2]);
			}
		}
		vb.Update(&vertices[0], vertices.size());
	}
	static std::unique_ptr<mesh> meshesfrommd5(std::string meshfile, md5::model& m, std::vector<mesh::Vertex>& vertices)
	{
		Tokenizer md5tokenizer(meshfile, md5::whitespace, md5::delimiters);
		md5::meshfile::parse(md5tokenizer, m);
		for(auto meshit = m.meshes.begin(); meshit != m.meshes.end(); ++meshit)
		{
			for(auto trisit = meshit->triangles.begin(); trisit != meshit->triangles.end(); ++trisit)
			{
				mesh::Vertex v0;
				v0.pos = md5::meshfile::getfinalpos(meshit->vertices[trisit->v0], *meshit, m.joints);
				v0.texcoord = meshit->vertices[trisit->v0].texcoord;
				mesh::Vertex v1;
				v1.pos = md5::meshfile::getfinalpos(meshit->vertices[trisit->v1], *meshit, m.joints);
				v1.texcoord = meshit->vertices[trisit->v1].texcoord;
				mesh::Vertex v2;
				v2.pos = md5::meshfile::getfinalpos(meshit->vertices[trisit->v2], *meshit, m.joints);
				v2.texcoord = meshit->vertices[trisit->v2].texcoord;
				vertices.push_back(v0);
				vertices.push_back(v1);
				vertices.push_back(v2);
				
			} 
		}
		std::unique_ptr<mesh> model(new mesh(description, &vertices[0], vertices.size()));
		return model;
		
	}
	void Draw() const
	{
		vb.Draw();
	}
};
const VertexBuffer::InputElementDescription mesh::description[] { { "in_position", 3, sizeof(glm::vec3) },
                                                                   { "in_normal",   3, sizeof(glm::vec3) },
                                                                   { "in_texcoord", 2, sizeof(glm::vec2) },
                                                                   { "", 0, 0 } };
class effectinput : public object
{
private:
	mesh& m;
	std::vector<Texture*> textures;
public:
	effectinput(std::vector<Texture*> textures)
	: m(mesh::square())
	, textures(textures)
	{
	}
	void Draw(ShaderProgram& sp) const
	{
		for(size_t i = 0; i < textures.size(); i++)
		{
			textures[i]->Bind(i);
		}
		m.Draw();
	}
};


class scene : public object
{
private:
	std::vector<object*> objs;
public:
	void add(object* obj)
	{
		objs.push_back(obj);
	}
	void Draw(ShaderProgram& sp) const
	{
		for(auto it = objs.begin(); it != objs.end(); ++it)
		{
			(*it)->Draw(sp);
		}
	}
};

class entity : public object
{
public:
	mesh& model;
	glm::vec3 position;
	glm::quat rotation;
	Texture& ctex;
	entity(mesh& model, Texture& ctex)
	: model(model)
	, ctex(ctex)
	{
	}
	void Draw(ShaderProgram& sp) const
	{
		sp.Set("world", &(glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation))[0][0]);
		ctex.Bind(0);
		model.Draw();
	}
	
};

class View
{
public:
	glm::mat4 projection;
	glm::mat4 view;
	View(glm::mat4 projection, glm::mat4 view)
	: projection(projection)
	, view(view)
	{
	}
	virtual glm::mat4 GetViewMatrix() const = 0;
};

class EffectsStep;
class RenderStep
{
friend class EffectsStep;
protected:
	size_t width, height;
	VertexShader vs;
	FragmentShader fs;
	ShaderProgram sp;
	std::unique_ptr<RenderTarget> rt;
public:
	struct Descriptor
	{
		enum shadertype {
			VS,
			FS
		};
		enum iotype {
			OUTPUT,
			INPUT
		};
		void set(shadertype t, std::string val)
		{
			switch(t)
			{
			case VS:
				vs = val;
			break;
			case FS:
				fs = val;
			break;
			default:
				assert(false);
			break;
			};
		}
		void add(iotype t, std::string val)
		{
			switch(t)
			{
			case INPUT:
				inputs.push_back(val);
			break;
			case OUTPUT:
				outputs.push_back(val);
			break;
			default:
				assert(false);
			break;
			};
		}
		std::string vs;
		std::string fs;
		std::vector<std::string> outputs;
		std::vector<std::string> inputs;
	};
	RenderStep(size_t width, size_t height, const Descriptor& descriptor)
	: width(width), height(height)
	, vs(descriptor.vs)
	, fs(descriptor.fs)
	, sp(vs, fs, mesh::description)
	{
		sp.Use();
		for(size_t i = 0; i < descriptor.inputs.size(); i++)
		{
			sp.SetTexture(descriptor.inputs[i].c_str(), i);
		}
		std::vector<Texture*> output;
		for(size_t i = 0; i < descriptor.outputs.size(); i++)
		{
			output.push_back(Res<Texture>::load(descriptor.outputs[i]));
		}
		rt.reset(new RenderTarget(width, height, output));
	}
	void Step(const View& view, object& obj)
	{
		rt->Activate();
		sp.Use();
		sp.Set("projection", &view.projection[0][0]);
		sp.Set("view", &view.GetViewMatrix()[0][0]);
		obj.Draw(sp);
	}
};

class EffectsStep
{
private:
	RenderStep rs;
	effectinput input;
public:
	EffectsStep(size_t width, size_t height, const RenderStep::Descriptor& descriptor, std::vector<Texture*> textures)
	: rs(width, height, descriptor)
	, input(textures)
	{
	}
	void Step()
	{
		rs.rt->Activate();
		rs.sp.Use();
		input.Draw(rs.sp);
	}
};

#include <Rocket/Core/FileInterface.h>
#include <Rocket/Core/RenderInterface.h>
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/Core.h>
namespace ui
{

class UiFileInterface : public Rocket::Core::FileInterface
{
public:
	const Rocket::Core::String& root;
	UiFileInterface(const Rocket::Core::String& root)
	: root(root)
	{
	}
	~UiFileInterface()
	{
	}
	Rocket::Core::FileHandle Open(const Rocket::Core::String& path)
	{
		FILE* fp = fopen((root + path).CString(), "rb");
		if (fp != NULL) return (Rocket::Core::FileHandle) fp;
		fp = fopen(path.CString(), "rb");
		return (Rocket::Core::FileHandle) fp;
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

class UiRenderer : public Rocket::Core::RenderInterface
{
private:
	size_t width, height;
	std::map<Rocket::Core::TextureHandle, std::unique_ptr<Texture>> texmap;
public:
	UiRenderer(size_t width, size_t height)
	: width(width)
	, height(height)
	{
	}
	void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
	{
//		glPushMatrix();
//		glTranslatef(translation.x, translation.y, 0);
		static VertexBuffer::InputElementDescription description[] { { "in_position", 3, sizeof(Rocket::Core::Vector2f) },
                                                                             { "in_color",   1, sizeof(Rocket::Core::Colourb) },
                                                                             { "in_texcoord", 1, sizeof(Rocket::Core::Vector2f) },
                                                                             { "", 0, 0 } };
		VertexBuffer vb(description, vertices, num_vertices, GL_TRIANGLES);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if(!texture) throw new std::exception();
		texmap.find(texture)->second->Bind(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		vb.DrawIndexed(*indices, num_indices);


//		glPopMatrix();
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
		glScissor(x, this->height - (y + height), width, height);
	}
	bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
	{
		texmap.insert(std::make_pair(texture_handle, new Texture(source_dimensions.x, source_dimensions.y, (unsigned char*)source)));
		return true;
	}
	void ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
	{
		texmap.erase(texture_handle);
	}
};

class ui
{
private:
	UiRenderer renderer;
	UiFileInterface fileinterface;
	Rocket::Core::Context* Context;
public:
	ui(size_t width, size_t height)
	: renderer(width, height)
	, fileinterface("resources/ui")
	{
		Rocket::Core::SetFileInterface(&fileinterface);
		Rocket::Core::SetRenderInterface(&renderer);

		if(!Rocket::Core::Initialise())
			throw new std::exception();

		Rocket::Core::FontDatabase::LoadFontFace("Delicious-Bold.otf");
		Rocket::Core::FontDatabase::LoadFontFace("Delicious-BoldItalic.otf");
		Rocket::Core::FontDatabase::LoadFontFace("Delicious-Italic.otf");
		Rocket::Core::FontDatabase::LoadFontFace("Delicious-Roman.otf");

		Context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(width, height));

		//Rocket::Debugger::Initialise(Context);

		Rocket::Core::ElementDocument *Document = Context->LoadDocument("demo.rml");

		Document->Show();
		Document->RemoveReference();
	}
	void Update()
	{
		Context->Render();
		Context->Update();
	}
};

/*
class UIStep : public RenderStep
{
private:
	UiRenderer renderer;
	UiFileInterface fileinterface;
	Rocket::Core::Context* Context;
public:
	UIStep(size_t width, size_t height, const Descriptor& descriptor)
	: RenderStep(width, height, descriptor)
	, renderer(width, height)
	, fileinterface("resources/ui/")
	{
		Rocket::Core::SetFileInterface(&fileinterface);
		Rocket::Core::SetRenderInterface(&renderer);

		if(!Rocket::Core::Initialise())
			throw new std::exception();

		Rocket::Core::FontDatabase::LoadFontFace("Delicious-Bold.otf");
		Rocket::Core::FontDatabase::LoadFontFace("Delicious-BoldItalic.otf");
		Rocket::Core::FontDatabase::LoadFontFace("Delicious-Italic.otf");
		Rocket::Core::FontDatabase::LoadFontFace("Delicious-Roman.otf");

		Context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(width, height));

		//Rocket::Debugger::Initialise(Context);

		Rocket::Core::ElementDocument *Document = Context->LoadDocument("demo.rml");

		Document->Show();
		Document->RemoveReference();
	}
	void Step(const View& view, const std::vector<entity*> objects) 
	{
		rt->Activate();
		sp.Use();
//		sp.Set("world", &(objects[j]->getworld()[0][0]));
		Context->Render();
		Context->Update();
	}
};*/
}

class RenderPipeline
{
private:
	std::unique_ptr<RenderStep> geometry;
	std::unique_ptr<RenderStep> ui;
	std::unique_ptr<EffectsStep> deferred;
	size_t width, height;
public:
	RenderPipeline(size_t width, size_t height)
	: width(width)
	, height(height)
	{
		RenderStep::Descriptor geometrydescriptor;
		geometrydescriptor.set(RenderStep::Descriptor::VS, "resources/shaders/geometry.vs");
		geometrydescriptor.set(RenderStep::Descriptor::FS, "resources/shaders/geometry.fs");
		geometrydescriptor.add(RenderStep::Descriptor::INPUT, "modeltex");
		geometrydescriptor.add(RenderStep::Descriptor::OUTPUT, "position");
		geometrydescriptor.add(RenderStep::Descriptor::OUTPUT, "color");
		geometrydescriptor.add(RenderStep::Descriptor::OUTPUT, "normal");

		RenderStep::Descriptor deferreddescriptor;
		deferreddescriptor.set(RenderStep::Descriptor::VS, "resources/shaders/deferred.vs");
		deferreddescriptor.set(RenderStep::Descriptor::FS, "resources/shaders/deferred.fs");
		deferreddescriptor.add(RenderStep::Descriptor::INPUT, "positiontex");
		deferreddescriptor.add(RenderStep::Descriptor::INPUT, "colortex");
		deferreddescriptor.add(RenderStep::Descriptor::INPUT, "normaltex");
		deferreddescriptor.add(RenderStep::Descriptor::INPUT, "uitex");

		RenderStep::Descriptor uidescr;
		uidescr.set(RenderStep::Descriptor::VS, "resources/shaders/ui.vs");
		uidescr.set(RenderStep::Descriptor::FS, "resources/shaders/ui.fs");
		uidescr.add(RenderStep::Descriptor::OUTPUT, "ui");

		std::vector<Texture*> textures;
		textures.push_back(Res<Texture>::load("position", new Texture(width, height)));
		textures.push_back(Res<Texture>::load("color", new Texture(width, height)));
		textures.push_back(Res<Texture>::load("normal", new Texture(width, height)));
		textures.push_back(Res<Texture>::load("ui", new Texture(width, height)));

		geometry.reset(new RenderStep(width, height, geometrydescriptor));
		ui.reset(new RenderStep(width, height, uidescr));
		deferred.reset(new EffectsStep(width, height, deferreddescriptor, textures));
	};
	void Step(const View& view, object& obj)
	{
		geometry->Step(view, obj);
		ui->Step(view, obj);
		deferred->Step();
	}
};

class Camera : public View
{
public:
	Camera(size_t width, size_t height)
	: View(glm::perspective(60.0f, (float)width / (float)height, 1.0f, 1000.0f), glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)))
	, quit(false)
	, position(glm::vec3(40.0f, 35.0f, 100.0f))
	, up(false), down(false), left(false), right(false)
	, w(false), a(false), s(false), d(false)
	, speed(2.0)
	{
	}
	void Update()
	{
		left = glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS;
		right = glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS;
		up = glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS;
		down = glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS;
		w = glfwGetKey('W') == GLFW_PRESS;
		a = glfwGetKey('A') == GLFW_PRESS;
		s = glfwGetKey('S') == GLFW_PRESS;
		d = glfwGetKey('D') == GLFW_PRESS;
		quit = glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED);
		Recalculate();
	}

	void Recalculate()
	{
		if(left) MoveX(-0.2 * speed);
		if(right) MoveX(0.2 * speed);
		if(up) MoveZ(0.2 * speed);
		if(down) MoveZ(-0.2 * speed);
		if(w) RotateX(1.0 * speed);
		if(s) RotateX(-1.0 * speed);
		if(a) RotateY(1.0 * speed);
		if(d) RotateY(-1.0 * speed);
	}
	
	glm::mat4 GetViewMatrix() const
	{
		return glm::inverse(glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation));
	}

	void MoveX(float xmmod)
	{
		position += orientation * glm::vec3(xmmod, 0.0f, 0.0f);
	}
	 
	void MoveZ(float zmmod)
	{
		position += orientation * glm::vec3(0.0f, 0.0f, -zmmod);
	}
	 
	void RotateX(float xrmod)
	{
		orientation = orientation * glm::angleAxis(xrmod, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	 
	void RotateY(float yrmod)
	{
		orientation = orientation * glm::angleAxis(yrmod, glm::vec3(0.0f, 1.0f, 0.0f));
	}
public:
	bool quit;
private:
	glm::vec3 position;
	glm::quat orientation;
	bool up, down, left, right;
	bool w, a, s, d;
	float speed;
};

int main()
{
	unsigned int width = 800;
	unsigned int height = 600;
	Window_ window(width, height);
	Camera camera(width, height);

	md5::anim a;
	mesh::animfrommd5("spikes/marine.md5anim", a);
	md5::model m; 
	std::vector<mesh::Vertex> vertices;
	std::unique_ptr<mesh> md5model(mesh::meshesfrommd5("spikes/marine.md5mesh", m, vertices));
	Texture::ImageData imagedata{IL_PNG, File::read("marine.png")};
	entity md5(*(md5model.get()), *Res<Texture>::load("marine.png", new Texture(width, height, imagedata)));
	scene s;
	s.add(&md5);
	RenderPipeline pipeline(width, height);
	size_t framenr = 0;
	size_t counter = 0;
	timer t;
	ui::ui ui_(width, height);
	while(!camera.quit)
	{
		ui_.Update();
		pipeline.Step(camera, s);
		window.Swap();
		camera.Update();
		//animation for now seems to have 60 frames.
		md5model->nextmd5frame(m, a, vertices, framenr % 60);
		if(counter++ == 10)
		{
			framenr++;
			counter = 0;
		}
	}
	return 0;
}


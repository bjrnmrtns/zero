#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
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
#include "webserver.cpp"
#include "GeneralException.cpp"
#include <thread>
#include <sstream>
#include <json_spirit/json_spirit_writer_template.h>
#define JSON_SPIRIT_VALUE_ENABLED
#ifndef JSON_SPIRIT_VALUE_ENABLED
#error Please define JSON_SPIRIT_VALUE_ENABLED for the Value type to be enabled 
#endif
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
	static T* load(std::string name, std::unique_ptr<T> val)
	{
		if(data.find(name) != data.end()) throw GeneralException("Resource: " + name + ", already loaded");
		T* ptr = val.get();
		data.insert(std::make_pair(name, std::move(val)));
		return ptr;
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
	VertexBuffer(const InputElementDescription description[], const T vertexData[], size_t count)
	: stride(sizeof(T))
	, count(count)
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
		glBufferData(GL_ARRAY_BUFFER, stride * count, vertexData, GL_STATIC_DRAW);
	}
	void Draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, count);
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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
		}
		ImageData save()
		{
			//TODO: Remove these magic numbers
			const int maxwidth = 2048;
			const int maxheight = 2048;
			static unsigned char data[maxwidth * maxheight * 3];
			ilBindImage(id);
			texture.Bind(0);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, &data);
			assert(ilSetData(data));
			ImageData imagedata;
			imagedata.type = IL_PNG;
			imagedata.blob.size = ilSaveL(IL_PNG, data, maxwidth * maxheight * 3);
			unsigned char* savedata = (unsigned char*)malloc(imagedata.blob.size);
			imagedata.blob.buf.reset(savedata);
			memcpy(savedata, data, imagedata.blob.size);
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
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
	RenderTarget(unsigned int width, unsigned int height, std::vector<std::pair<std::string, Texture*>>& targets)
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
				targets[i].second->Attach(i);
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

class Model
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
	Model(const VertexBuffer::InputElementDescription description[], const Vertex vertices[], const size_t size)
	: vb(description, vertices, size)
	{
	}
	static Model& square()
	{
		static const Vertex vertices[] { { glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
		                                 { glm::vec3(-1,  1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) },
		                                 { glm::vec3( 1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
		                                 { glm::vec3(-1,  1, 1), glm::vec3(0, 0,-1), glm::vec2(0, 1) },
		                                 { glm::vec3( 1,  1, 1), glm::vec3(0, 0,-1), glm::vec2(1, 1) },
		                                 { glm::vec3( 1, -1, 1), glm::vec3(0, 0,-1), glm::vec2(1, 0) } };
		static Model model(description, vertices, sizeof(vertices)/sizeof(float));
		return model;
	};
	void Draw()
	{
		vb.Draw();
	}
};
const VertexBuffer::InputElementDescription Model::description[] { { "in_position", 3, sizeof(glm::vec3) },
                                                                   { "in_normal",   3, sizeof(glm::vec3) },
                                                                   { "in_texcoord", 2, sizeof(glm::vec2) },
                                                                   { "", 0, 0 } };

class RenderStep
{
private:
	size_t width, height;
	VertexShader vs;
	FragmentShader fs;
	ShaderProgram sp;
	Model& square;
	std::vector<std::pair<std::string, Texture*>> inputs;
	std::vector<std::pair<std::string, Texture*>> output;
	std::unique_ptr<RenderTarget> rt;
public:
	struct Descriptor
	{
		struct io
		{
			std::string name;
			std::string id;
		};
		std::string vs;
		std::string fs;
		std::vector<io> inputs, outputs;
	};
	RenderStep(size_t width, size_t height, const Descriptor& descriptor)
	: width(width), height(height)
	, vs(descriptor.vs)
	, fs(descriptor.fs)
	, sp(vs, fs, Model::description)
	, square(Model::square())
	{
		for(size_t i = 0; i < descriptor.inputs.size(); i++)
		{
			addinput(descriptor.inputs[i].name, descriptor.inputs[i].id);
		}
		for(size_t i = 0; i < descriptor.outputs.size(); i++)
		{
			addoutput(descriptor.outputs[i].name, descriptor.outputs[i].id);
		}
		rt.reset(new RenderTarget(width, height, output));
	}
	void Step()
	{
		sp.Use();
		for(size_t i = 0; i < inputs.size(); i++)
		{
			inputs[i].second->Bind(i);
		}
		rt->Activate();
		square.Draw();
	}
private:
	void addinput(std::string name, std::string id)
	{
		inputs.push_back(std::make_pair(name, Res<Texture>::load(id)));
		sp.Use();
		for(size_t i = 0; i < inputs.size(); i++)
		{
			sp.SetTexture(inputs[i].first.c_str(), i);
		}
	}
	void addoutput(std::string name, std::string id)
	{
		output.push_back(std::make_pair(name, Res<Texture>::load(id)));
	}
};

class View
{
public:
	View(glm::mat4 projection, glm::mat4 view)
	: view(view)
	, projection(projection)
	{
	}
	glm::mat4 projection;
	glm::mat4 view;
};

class Renderable
{
public:
	virtual void Draw() = 0;
};

class RenderPipeline
{
private:
	std::vector<std::unique_ptr<RenderStep>> steps;
	size_t width, height;
public:
	RenderPipeline(size_t width, size_t height)
	: width(width)
	, height(height)
	{
		RenderStep::Descriptor effectA { "resources/shaders/null.vs", "resources/shaders/null.fs"};//,
		RenderStep::Descriptor::io inputA{"modeltex", "pic.png"};
		RenderStep::Descriptor::io outputA1{"output", "A1"};
		RenderStep::Descriptor::io outputA2{"output", "A2"};
		effectA.inputs.push_back(inputA);
		effectA.outputs.push_back(outputA1);
		effectA.outputs.push_back(outputA2);

		RenderStep::Descriptor effectB { "resources/shaders/stepB.vs", "resources/shaders/stepB.fs"};//,
		RenderStep::Descriptor::io inputB{"modeltex", "A1"};
		effectB.inputs.push_back(inputB);

		Texture::ImageData imagedata{IL_PNG, File::read("pic.png")};
		Res<Texture>::load("pic.png", std::unique_ptr<Texture>(new Texture(width, height, imagedata)));
		Res<Texture>::load("A1", std::unique_ptr<Texture>(new Texture(width, height)));
		Res<Texture>::load("A2", std::unique_ptr<Texture>(new Texture(width, height)));

		steps.push_back(std::unique_ptr<RenderStep>(new RenderStep(width, height, effectA)));
		steps.push_back(std::unique_ptr<RenderStep>(new RenderStep(width, height, effectB)));
	};
	void Step(const View& view, const Renderable& renderable)
	{
		for(size_t i = 0; i < steps.size(); i++)
		{
			steps[i]->Step();
		}
	}
};

class Camera : public View
{
private:
public:
	Camera(size_t width, size_t height)
	: View(glm::mat4(1.0f), glm::perspective(60.0f, (float)width / (float)height, 1.0f, 10000.0f))
	{
	}
};

class Cube : public Renderable
{
public:
	void Draw()
	{
	}
};

/*static std::map<std::string, Blob> textures;
class TextureHandler : public net::http::server::handler
{
public:
	net::http::response handle(net::http::request& req, net::socket& sock)
	{
		net::http::response resp(sock);
		//TODO: not thread safe (assuming nobody writes to this image)
		resp.setcontent(textures.find("A1")->second);
		return resp;
	}
};
*/
int main()
{
/*	json_spirit::Object addr;
	json_spirit::Array textu;
	addr.push_back(json_spirit::Pair( "texture", "texture/1"));
	textu.push_back("texture/2");
	textu.push_back("texture/3");
	addr.push_back(json_spirit::Pair("textures", textu));
	std::stringstream os;
	std::cout << json_spirit::write_string(json_spirit::Value(addr), json_spirit::pretty_print);

	TextureHandler texhandler;
	net::http::server::Instance().registeruri("/textures", &texhandler);
	std::thread serverthread(std::ref(net::http::server::Instance()));
	serverthread.detach();
*/
	unsigned int width = 1024;
	unsigned int height = 768;
	Window_ window(width, height);
	RenderPipeline pipeline(width, height);
	bool running = true;
	Camera camera(width, height);
	Cube cube;
	while(running)
	{
		pipeline.Step(camera, cube);
		window.Swap();

		running = !glfwGetKey(GLFW_KEY_ESC) &&
		          glfwGetWindowParam(GLFW_OPENED);
/*		for(auto &it : Res<Texture>::data) {
			textures.insert(std::make_pair(it.first, it.second->save().blob));
		}*/

	}
/*	File::write("rendtexA.png", Res<Texture>::data.find("A1")->second->save().blob);
	File::write("rendtexB.png", Res<Texture>::data.find("A2")->second->save().blob);*/
	return 0;
}


#include <GL/glew.h>
#include <SDL/SDL.h>
#include <exception>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>

static const unsigned int screenWidth = 512;
static const unsigned int screenHeight = 512;

static const char *deferredFS =
"#version 150\n"
"\n"
"uniform sampler2D modeltex;\n"
"in vec3 ex_Position;\n"
"in vec3 ex_Normal;\n"
"in vec2 ex_texcoord;\n"
"\n"
"void main(void)\n"
"{\n"
"  gl_FragData[0] = texture2D(modeltex, ex_texcoord);\n"
"  gl_FragData[1] = vec4(ex_Normal, 1.0);\n"
"}\n";


static const char *initialFS =
"#version 150\n"
"\n"
"uniform sampler2D modeltex;\n"
"in vec3 ex_Position;\n"
"in vec3 ex_Normal;\n"
"in vec2 ex_texcoord;\n"
"\n"
"void main(void)\n"
"{\n"
"  gl_FragData[0] = texture2D(modeltex, ex_texcoord);\n"
"}\n";

static const char *initialVS =
"#version 150\n"
"uniform mat4 projectionMatrix;\n"
"uniform mat4 viewMatrix;\n"
"uniform mat4 modelMatrix;\n"
"\n"
"in vec3 in_Position;\n"
"in vec3 in_Normal;\n"
"in vec2 in_Texcoord;\n"
"out vec3 pass_Position;\n"
"out vec3 pass_Normal;\n"
"out vec2 pass_texcoord;\n"
"\n"
"void main()\n"
"{\n"
"  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);\n"
"  pass_Position = gl_Position.xyz;\n"
"  pass_Normal = in_Normal;\n"
"  pass_texcoord = in_Texcoord;\n"
"}\n";

static const char *initialGS =
"#version 150\n"
"\n"
"layout(triangles) in;\n"
"layout(triangle_strip, max_vertices=3) out;\n"
"in vec3 pass_Position[];\n"
"in vec3 pass_Normal[];\n"
"in vec2 pass_texcoord[];\n"
"out vec3 ex_Position;\n"
"out vec3 ex_Normal;\n"
"out vec2 ex_texcoord;\n"
"\n"
"void main()\n"
"{\n"
"  for(int i = 0; i < 3; ++i)\n"
"  {\n"
"      ex_Position = pass_Position[i];\n"
"      ex_Normal = pass_Normal[i];\n"
"      ex_texcoord = pass_texcoord[i];\n"
"      gl_Position = gl_in[i].gl_Position;\n"
"      EmitVertex();\n"
"  }\n"
"  EndPrimitive();\n"
"}\n";

static const char *normalGS =
"#version 150\n"
"\n"
"layout(points) in;\n"
"layout(line_strip, max_vertices=2) out;\n"
"in vec3 pass_Position[];\n"
"in vec3 pass_Normal[];\n"
"in vec2 pass_texcoord[];\n"
"out vec3 ex_Position;\n"
"out vec3 ex_Normal;\n"
"out vec2 ex_texcoord;\n"
"\n"
"void main()\n"
"{\n"
"  gl_Position = gl_in[0].gl_Position;\n"
"  EmitVertex();\n"
"  gl_Position = gl_in[0].gl_Position + vec4(pass_Normal[0], 1.0);"// + pass_Normal[0];\n"
"  EmitVertex();\n"
"  EndPrimitive();\n"
"}\n";

static const char *normalFS =
"#version 150\n"
"\n"
"void main(void)\n"
"{\n"
"  gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);\n"
"  gl_FragData[1] = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";



class ShaderCompileException : public std::exception
{
public:
	ShaderCompileException(const std::string &errors)
		: errors(errors)
	{}
	~ShaderCompileException() throw () {}
	const char *what() const throw() { return errors.c_str(); }
private:
	const std::string errors;
};
		
class Shader
{
public:
	Shader(const char* source, int type) : type(type)
	{
		id = glCreateShader(type);
		glShaderSource(id, 1, &source, 0);
		glCompileShader(id);
		int ok = true;
		glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			int length = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char errors[length];
			glGetShaderInfoLog(id, length, 0, errors);
			throw ShaderCompileException(std::string(errors));
		}
	}
	virtual ~Shader()
	{
		glDeleteShader(id);
	}
	int GetId() { return id; }
private:
	int id;
	int type;
};

class VertexShader : public Shader
{
public:
	VertexShader(const char* source) : Shader(source, GL_VERTEX_SHADER) {}
};

class GeometryShader : public Shader
{
public:
	GeometryShader(const char* source) : Shader(source, GL_GEOMETRY_SHADER) {}
};


class FragmentShader : public Shader
{
public:
	FragmentShader(const char* source) : Shader(source, GL_FRAGMENT_SHADER) {}
};

class ShaderProgram
{
public:
	ShaderProgram(VertexShader &vertexShader, GeometryShader &geometryShader, FragmentShader &fragmentShader)
	: vertexShader(vertexShader)
	, geometryShader(geometryShader)
	, fragmentShader(fragmentShader)
	{
		id = glCreateProgram();
		glAttachShader(id, vertexShader.GetId());
		glAttachShader(id, geometryShader.GetId());
		glAttachShader(id, fragmentShader.GetId());
		
		glBindAttribLocation(id, 0, "in_Position");
		glBindAttribLocation(id, 1, "in_Normal");
		glBindAttribLocation(id, 2, "in_Texcoord");
		glLinkProgram(id);
	}
	~ShaderProgram()
	{
		glDetachShader(id, fragmentShader.GetId());
		glDetachShader(id, geometryShader.GetId());
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
	int id;
	VertexShader& vertexShader;
	GeometryShader& geometryShader;
	FragmentShader& fragmentShader;
};

class Renderable
{
public:
	virtual void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES) = 0;
	virtual ~Renderable() {};
};

class VertexBuffer : public Renderable
{
public:
	struct AttributeReference
	{
		int offset;
		int count;
	};

	template <typename T>
	VertexBuffer(AttributeReference *refs, const T *vertexData, size_t count)
	: stride(sizeof(T))
	, count(count)
	, refs(refs)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		for(int i = 0; refs[i].count; ++i)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, refs[i].count, GL_FLOAT, GL_FALSE, stride, (void *)refs[i].offset);
		}
		glBufferData(GL_ARRAY_BUFFER, stride * count, vertexData, GL_STATIC_DRAW);
	}
	void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES)
	{
		shaderProgram.Use();
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glDrawArrays(type, 0, count);
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
	AttributeReference *refs;
};

class Texture
{
public:
	Texture(unsigned int width, unsigned int height)
	{
		Construct(width, height, (void*)0);
	}
	Texture(unsigned int size, int nrOfBlocks)
	{
		struct
		{
			unsigned char R, G, B;
		} data[size][size];

		unsigned int freq = size / nrOfBlocks;
		for(unsigned int x = 0; x < size; x++)
		{
			for(unsigned int y = 0; y < size; y++)
			{
				if( ((x / freq) + (y / freq)) % 2)
				{
					data[x][y].R = 255;
					data[x][y].G = 255;
					data[x][y].B = 255;
				}
				else
				{
					data[x][y].R = 0;
					data[x][y].G = 0;
					data[x][y].B = 0;
				}
			}
		}


		Construct(size, size, (void*)data);
	}
	void Construct(unsigned int width, unsigned int height, void *data)
	{
		textureUnitId = GetTextureUnitId();
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	}
	void Bind(ShaderProgram& prog, const std::string& name)
	{
		int error;
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		if (prog.SetTexture(name.c_str(), textureUnitId))
		{
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
			glActiveTexture(GL_TEXTURE0+textureUnitId);
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
			glBindTexture(GL_TEXTURE_2D, id);
		error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		}
        }
	void SetAsAttachment(unsigned int count)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + count, id, 0);
	}
	~Texture()
	{
		glDeleteTextures(1, &id);
	}
	int GetTextureUnitId()
	{
		static int curtexUnit = -1;
		// TODO: max textureunits not necesseraly 16.
		if(curtexUnit > 15) throw 2;
		return (++curtexUnit % 16);
	}
private:
	unsigned int id;
	int textureUnitId;
};

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
	struct vertex
	{
		float x, y, z;
		float nx, ny, nz;
		float s0, t0;
	};
	Model(vertex *vertices, int vertexcount)
	: vertexBuffer(modelRefs, vertices, vertexcount)
	{
	}
	void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES)
	{
		vertexBuffer.Draw(shaderProgram, type);
	}
	static Model& Square()
	{
		static vertex v[] = {
			{ -1, -1, 1,  0,  0, 1,  0,  0 },
			{ -1,  1, 1,  0,  0, 1,  0,  1 },
			{  1, -1, 1,  0,  0, 1,  1,  0 },

			{ -1,  1, 1,  0,  0, -1,  0,  1 },
			{  1,  1, 1,  0,  0, -1,  1,  1 },
			{  1, -1, 1,  0,  0, -1,  1,  0 },

		};
		static Model square(v, sizeof(v)/sizeof(vertex));
		return square;
	}
	static Model& Cube()
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
private:
	VertexBuffer vertexBuffer;
};

class GLWindow
{
public:
	GLWindow()
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::printf("SDL_Init failed\n");
			throw 1;
		}

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
		mainwindow = SDL_CreateWindow("opengl_spike", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
		if (!mainwindow)
		{
			std::printf("Create Window failed\n");
			throw 1;
		}

		maincontext = SDL_GL_CreateContext(mainwindow);
		glewInit();
		{ int error = glGetError(); if (error) { printf("%s:%d: Error %d\n", __FILE__, __LINE__, error); abort(); } }
		SDL_GL_SetSwapInterval(1);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
	        glClearDepth(1.0f);
       		glEnable(GL_DEPTH_TEST);
	        glDepthFunc(GL_LEQUAL);

		glViewport(0, 0, screenWidth, screenHeight);
	}
	void Swap()
	{
		SDL_GL_SwapWindow(mainwindow);
	}
	~GLWindow()
	{
		SDL_GL_DeleteContext(maincontext);
		SDL_DestroyWindow(mainwindow);
		SDL_Quit();
	}
private:
	SDL_WindowID mainwindow;
	SDL_GLContext maincontext;
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
	RenderTarget(const unsigned int width, const unsigned int height)
	: fbo(0)
	, rbo (0)
	, width(width)
	, height(height)
	{
	}
	~RenderTarget()
	{
		if(targets.size() > 0)
		{
			glDeleteRenderbuffers(1, &rbo);
			glDeleteFramebuffers(1, &fbo);
		}
	}
	void AddTarget(Texture &target)
	{
		if(targets.size() == 0)
		{
			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		target.SetAsAttachment(targets.size());
		targets.push_back(&target);
	}
	void Activate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if(targets.size() > 0)
		{
			glDrawBuffers(targets.size(), Attachments);
			for(unsigned int i=0; i < targets.size(); i++)
			{
				targets[i]->SetAsAttachment(i);
			}
		}
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
private:
	unsigned int fbo;
	unsigned int rbo;
	unsigned int width;
	unsigned int height;
	std::vector<Texture*> targets;
};

class Window
{
public:
	Window()
	: window()
	, vertexShader(initialVS)
	, geometryShader(initialGS)
	, fragmentShader(initialFS)
	, normalVertexShader(initialVS)
	, normalGeometryShader(normalGS)
	, normalFragmentShader(normalFS)
	, deferredFragmentShader(deferredFS)
	, deferredShaderProgram(vertexShader, geometryShader, deferredFragmentShader)
	, shaderProgram(vertexShader, geometryShader, fragmentShader)
	, normalShaderProgram(normalVertexShader, normalGeometryShader, normalFragmentShader)
	, colorTexture(screenWidth, screenHeight)
	, depthTexture(screenWidth, screenHeight)
	, blockTexture(512, (int)4)
	, square(Model::Square())
	, cube(Model::Cube())
	, deferredTarget(screenWidth, screenHeight)
	, defaultTarget(screenWidth, screenHeight)
	{
		GameInit();
	}
	~Window()
	{
	}
	void GameInit()
	{
		deferredTarget.AddTarget(colorTexture);
		deferredTarget.AddTarget(depthTexture);
		glm::mat4 projectionMatrix = glm::gtc::matrix_transform::perspective(60.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.f);
		//glm::mat4 viewMatrix  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
		glm::mat4 viewMatrix  = glm::mat4(1.0f);

		deferredShaderProgram.Set("projectionMatrix", &projectionMatrix[0][0]);
		deferredShaderProgram.Set("viewMatrix", &viewMatrix[0][0]);
		normalShaderProgram.Set("projectionMatrix", &projectionMatrix[0][0]);
		normalShaderProgram.Set("viewMatrix", &viewMatrix[0][0]);

		glm::mat4 screenProjectionMatrix = glm::perspective(60.0f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.f);
		glm::mat4 screenModelMatrix  = glm::mat4(1.0f);
		shaderProgram.Set("projectionMatrix", &screenProjectionMatrix[0][0]);
		shaderProgram.Set("modelMatrix", &screenModelMatrix[0][0]);

	}
	void Render()
	{
		static float rotation = 100;
		rotation++;
		//glm::mat4 modelMatrix  = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(1.0f, 1.0f, 0.0f));
		glm::mat4 modelMatrix  = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -4.0f));
		deferredShaderProgram.Set("modelMatrix", &modelMatrix[0][0]);
		normalShaderProgram.Set("modelMatrix", &modelMatrix[0][0]);
		deferredTarget.Activate();
		blockTexture.Bind(deferredShaderProgram, "modeltex");
		cube.Draw(deferredShaderProgram);
		cube.Draw(normalShaderProgram, GL_POINTS);

		defaultTarget.Activate();
		glm::mat4 screenViewMatrix  = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -4.0f));
		shaderProgram.Set("viewMatrix", &screenViewMatrix[0][0]);
		colorTexture.Bind(shaderProgram, "modeltex");
		square.Draw(shaderProgram);
		screenViewMatrix  = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -4.0f));
		shaderProgram.Set("viewMatrix", &screenViewMatrix[0][0]);
		depthTexture.Bind(shaderProgram, "modeltex");
		square.Draw(shaderProgram);
		window.Swap();
	}
private:
	GLWindow window;
	VertexShader vertexShader;
	GeometryShader geometryShader;
	FragmentShader fragmentShader;
	VertexShader normalVertexShader;
	GeometryShader normalGeometryShader;
	FragmentShader normalFragmentShader;
	FragmentShader deferredFragmentShader;
	ShaderProgram deferredShaderProgram;
	ShaderProgram shaderProgram;
	ShaderProgram normalShaderProgram;
	Texture colorTexture;
	Texture depthTexture;
	Texture blockTexture;
	Model square;
	Model cube;
	RenderTarget deferredTarget;
	RenderTarget defaultTarget;
};

int main()
{
	try
	{
		Window window;
		int frameCounter = 0;
		while(frameCounter < 400)
		{
			frameCounter++;
			window.Render();
			SDL_Delay(10);
		}
	}
	catch(int rv)
	{
		return rv;
	}
	return 0;
}

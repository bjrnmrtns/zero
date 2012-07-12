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
#include <string.h>
#include <cstdlib>
#include <map>
#include "GeneralException.cpp"
#include "Blob.cpp"
		
static int theworld[128][128][128];

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
	static void unload(std::string name)
	{
		auto it = data.find(name);
		if(it != data.end()) data.erase(it);
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
		int type;
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
			glVertexAttribPointer(i, description[i].numberofelements, description[i].type, GL_FALSE, stride, (void *)offset);
			offset += description[i].elementsize;
		}
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

class Window_
{
public:
	Window_(size_t width, size_t height)
	{
		if(!glfwInit()) throw new GeneralException("glfwInit failed");
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
		glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		if(!glfwOpenWindow(width, height, 8, 8, 8, 0, 24, 8, GLFW_WINDOW))
		{
			glfwTerminate();
			throw new GeneralException("glfwOpenWindow failed");
		}
		glfwDisable(GLFW_MOUSE_CURSOR);
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

class mesh
{
public:
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texcoord;
		glm::vec3 color;
	};
private:
	VertexBuffer vb;
public:
	static const VertexBuffer::InputElementDescription description[];
	mesh(const VertexBuffer::InputElementDescription description[], const Vertex vertices[], const size_t size, unsigned int mode = GL_TRIANGLES)
	: vb(description, vertices, size, mode)
	{
	}
	static mesh& square()
	{
		static const Vertex vertices[] { { glm::vec3( 0, 0, 1), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
		                                 { glm::vec3( 0, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1) },
		                                 { glm::vec3( 1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
		                                 { glm::vec3( 0, 1, 1), glm::vec3(0, 0,-1), glm::vec2(0, 1) },
		                                 { glm::vec3( 1, 1, 1), glm::vec3(0, 0,-1), glm::vec2(1, 1) },
		                                 { glm::vec3( 1, 0, 1), glm::vec3(0, 0,-1), glm::vec2(1, 0) } };
		static mesh model(description, vertices, sizeof(vertices)/sizeof(Vertex));
		return model;
	};
	static mesh& cube()
	{
	glm::vec3 color(0.0f, 0.0f, 1.0f);
	static Vertex vertices[] = { { glm::vec3( 0,  1,  0), glm::vec3(-1,  0,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 0,  0,  1), glm::vec3(-1,  0,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 0,  0,  0), glm::vec3(-1,  0,  0),  glm::vec2(1,  0), color }, 

	                             { glm::vec3( 0,  0,  1), glm::vec3(-1,  0,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 0,  1,  0), glm::vec3(-1,  0,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 0,  1,  1), glm::vec3(-1,  0,  0),  glm::vec2(0,  1), color },

	                             { glm::vec3( 0,  1,  1), glm::vec3( 0,  0,  1),  glm::vec2(0,  0), color },
	                             { glm::vec3( 1,  0,  1), glm::vec3( 0,  0,  1),  glm::vec2(1,  1), color },
	                             { glm::vec3( 0,  0,  1), glm::vec3( 0,  0,  1),  glm::vec2(1,  0), color },

	                             { glm::vec3( 1,  0,  1), glm::vec3( 0,  0,  1),  glm::vec2(1,  1), color },
	                             { glm::vec3( 0,  1,  1), glm::vec3( 0,  0,  1),  glm::vec2(0,  0), color },
	                             { glm::vec3( 1,  1,  1), glm::vec3( 0,  0,  1),  glm::vec2(0,  1), color },

	                             { glm::vec3( 1,  0,  1), glm::vec3( 1,  0,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 1,  1,  0), glm::vec3( 1,  0,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 1,  0,  0), glm::vec3( 1,  0,  0),  glm::vec2(1,  0), color },

	                             { glm::vec3( 1,  1,  0), glm::vec3( 1,  0,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 1,  0,  1), glm::vec3( 1,  0,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 1,  1,  1), glm::vec3( 1,  0,  0),  glm::vec2(0,  1), color },

	                             { glm::vec3( 1,  1,  0), glm::vec3( 0,  0, -1),  glm::vec2(0,  0), color },
	                             { glm::vec3( 0,  0,  0), glm::vec3( 0,  0, -1),  glm::vec2(1,  1), color },
	                             { glm::vec3( 1,  0,  0), glm::vec3( 0,  0, -1),  glm::vec2(0,  1), color },

	                             { glm::vec3( 0,  0,  0), glm::vec3( 0,  0, -1),  glm::vec2(1,  1), color },
	                             { glm::vec3( 1,  1,  0), glm::vec3( 0,  0, -1),  glm::vec2(0,  0), color },
	                             { glm::vec3( 0,  1,  0), glm::vec3( 0,  0, -1),  glm::vec2(1,  0), color },

	                             { glm::vec3( 0,  1,  0), glm::vec3( 0,  1,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 1,  1,  1), glm::vec3( 0,  1,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 0,  1,  1), glm::vec3( 0,  1,  0),  glm::vec2(1,  0), color },

	                             { glm::vec3( 1,  1,  1), glm::vec3( 0,  1,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 0,  1,  0), glm::vec3( 0,  1,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 1,  1,  0), glm::vec3( 0,  1,  0),  glm::vec2(0,  1), color },

	                             { glm::vec3( 1,  0,  0), glm::vec3( 0, -1,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 0,  0,  1), glm::vec3( 0, -1,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 1,  0,  1), glm::vec3( 0, -1,  0),  glm::vec2(1,  0), color },

	                             { glm::vec3( 0,  0,  1), glm::vec3( 0, -1,  0),  glm::vec2(1,  1), color },
	                             { glm::vec3( 1,  0,  0), glm::vec3( 0, -1,  0),  glm::vec2(0,  0), color },
	                             { glm::vec3( 0,  0,  0), glm::vec3( 0, -1,  0),  glm::vec2(0,  1), color }
		};
		static mesh model(description, vertices, sizeof(vertices)/sizeof(Vertex));
		return model;
	}
	static mesh* blocks()
	{
		glm::vec3 colors[] { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
		size_t N = 0;
		for(size_t z = 0; z < 128; z++)
		{
			for(size_t y = 0; y < 128; y++)
			{
				for(size_t x = 0; x < 128; x++)
				{
					if((y == 0) || ((y == 1) && x == 3))
					{
						theworld[x][y][z] = 1;
						N++;
					}
				}
			}
		}
		const size_t size = 64;
		Vertex *vertices = new Vertex[3 * 12 * N];
		size_t i = 0;
		std::srand(time(0));
		for(size_t z = 0; z < size; z++)
		{
			for(size_t y = 0; y < size; y++)
			{
				for(size_t x = 0; x < size; x++)
				{
					glm::vec3 color = colors[std::rand() % (sizeof(colors)/sizeof(glm::vec3))];
					if(theworld[x][y][z])
					{
						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3(-1,  0,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3(-1,  0,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3(-1,  0,  0),  glm::vec2(1,  0), color }; 

						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3(-1,  0,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3(-1,  0,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3(-1,  0,  0),  glm::vec2(0,  1), color };

						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1),  glm::vec2(1,  0), color };

						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1),  glm::vec2(0,  1), color };

						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 1,  0,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 1,  0,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 1,  0,  0),  glm::vec2(1,  0), color };

						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 1,  0,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 1,  0,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 1,  0,  0),  glm::vec2(0,  1), color };

						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1),  glm::vec2(0,  1), color };

						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1),  glm::vec2(1,  0), color };

						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0),  glm::vec2(1,  0), color };

						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0),  glm::vec2(0,  1), color };

						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 0, -1,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0, -1,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0, -1,  0),  glm::vec2(1,  0), color };

						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0, -1,  0),  glm::vec2(1,  1), color };
						vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 0, -1,  0),  glm::vec2(0,  0), color };
						vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0, -1,  0),  glm::vec2(0,  1), color } ;
					}
				}
			}
		}
		mesh *m = new mesh(description, vertices, i);
		delete vertices;
		return m;
	}
	void Draw() const
	{
		vb.Draw();
	}
};
const VertexBuffer::InputElementDescription mesh::description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT },
                                                                  { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT },
                                                                  { "in_texcoord", 2, sizeof(glm::vec2), GL_FLOAT },
                                                                  { "in_color",    3, sizeof(glm::vec3), GL_FLOAT },
                                                                  { "", 0, 0, 0 } };

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

class Camera : public View
{
public:
	Camera(size_t width, size_t height)
	: View(glm::perspective(60.0f, (float)width / (float)height, 1.0f, 1000.0f), glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)))
	, quit(false)
	, position(glm::vec3(1.0f, 3.0f, 12.0f))
	, up(false), down(false), left(false), right(false)
	, w(false), a(false), s(false), d(false)
	, i(false), j(false), k(false), l(false)
	, speed(1.0)
	, space(false)
	, diffx(0)
	, diffy(0)
	, width(width)
	, height(height)
	{
		glfwSetMousePos(width/2, height/2);
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
		i = glfwGetKey('I') == GLFW_PRESS;
		j = glfwGetKey('J') == GLFW_PRESS;
		k = glfwGetKey('K') == GLFW_PRESS;
		l = glfwGetKey('L') == GLFW_PRESS;
		int xMouse;
		int yMouse;
		glfwGetMousePos(&xMouse, &yMouse);
		diffx = (width/2) - xMouse;
		diffy = (height/2) - yMouse;
		std::cout << diffx <<  " " << diffy << std::endl;
		space = glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS;
		quit = glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED);
		Recalculate();
		glfwSetMousePos(width/2, height/2);
	}

	void Recalculate()
	{
		if(a) MoveX(-0.2 * speed);
		if(d) MoveX(0.2 * speed);
		if(w) MoveZ(0.2 * speed);
		if(s) MoveZ(-0.2 * speed);
		RotateX(((float)diffy / 10.0) * speed);
		RotateY(((float)diffx / 10.0) * speed);
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
	bool i, j, k, l;
	bool space;
private:
	glm::vec3 position;
	glm::quat orientation;
	bool up, down, left, right;
	bool w, a, s, d;
	float speed;
	int diffx, diffy;
	size_t width, height;
};

class Player
{
public:
	mesh& cube;
	glm::vec3 position;
	float dy;
	Player()
	: cube(mesh::cube())
	, position(0.0f, 10.0f, 0.0f)
	{
	}
	
};

int main()
{
	unsigned int width = 800;
	unsigned int height = 600;
	Window_ window(width, height);
	Camera camera(width, height);
	VertexShader vs("resources/shaders/null.vs");
	FragmentShader fs("resources/shaders/null.fs");
	ShaderProgram sp(vs, fs, mesh::description);

	Player player;
	std::unique_ptr<mesh> m(mesh::blocks());
	while(!camera.quit)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sp.Use();
                sp.Set("projection", &camera.projection[0][0]);
                sp.Set("view", &camera.GetViewMatrix()[0][0]);
		sp.Set("world", &glm::mat4(1.0f)[0][0]);
		m->Draw();

		//
		static bool lastFrameWasStanding = false;
		glm::vec3 old = player.position;
		if (camera.space && lastFrameWasStanding) player.dy = 0.05;
		player.dy += -0.001;
		if (player.dy > 0.5) player.dy = 0.5;
		if (player.dy < -0.5) player.dy = -0.5;
		player.position.y += player.dy;

		if(camera.i) player.position.x += 0.02f;
		if(camera.k) player.position.x -= 0.02f;
		if(camera.j) player.position.z += 0.02f;
		if(camera.l) player.position.z -= 0.02f;
		

		// clip when going below zero or above 127
		if(player.position.x < 0.0) player.position.x = 0;
		if(player.position.y < 0.0) player.position.y = 0;
		if(player.position.z < 0.0) player.position.z = 0;
		if(player.position.x > 127.0) player.position.x = 127;
		if(player.position.y > 127.0) player.position.y = 127;
		if(player.position.z > 127.0) player.position.z = 127;

		glm::vec3 diff = player.position - old;
		static const int eps = 0.000001;
		lastFrameWasStanding = false;
		if(diff.y > eps)
		{
			if(theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0
			|| theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0)
			{
				player.position.y = old.y;
				player.dy = 0;
			}
		}
		if(diff.y < -eps)
		{
			if(theworld[(int)(player.position.x)][(int)(player.position.y)][(int)(player.position.z)] > 0
			|| theworld[(int)(player.position.x)][(int)(player.position.y)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y)][(int)(player.position.z)] > 0)
			{
				player.position.y = old.y;
				player.dy = 0;
				lastFrameWasStanding = true;
			}
		}
		if(diff.x > eps)
		{
			if(theworld[(int)(player.position.x + 0.8)][(int)(player.position.y)][(int)(player.position.z)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0)
			{
				player.position.x = old.x;
			}
		}
		if(diff.z > eps)
		{
			if(theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0)
			{
				player.position.z = old.z;
			}
		}
		if(diff.x < -eps)
		{
			if(theworld[(int)(player.position.x)][(int)(player.position.y)][(int)(player.position.z)] > 0
			|| theworld[(int)(player.position.x)][(int)(player.position.y)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z + 0.8)] > 0
			|| theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0)
			{
				player.position.x = old.x;
			}
		}
		if(diff.z < -eps)
		{
			if(theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0
			|| theworld[(int)(player.position.x)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0
			|| theworld[(int)(player.position.x + 0.8)][(int)(player.position.y + 0.8)][(int)(player.position.z)] > 0)
			{
				player.position.z = old.z;
			}
		}
		//

		glm::mat4 world = glm::translate(glm::mat4(1.0f), player.position);
		sp.Set("world", &world[0][0]);
		player.cube.Draw();
		window.Swap();
		camera.Update();
	}
	return 0;
}


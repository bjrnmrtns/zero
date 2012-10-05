#include <GL/glew.h>
#include <SDL/SDL.h>
#include <exception>
#include <cstdio>
#include <cmath>
#include <string>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <map>
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include "shader.h"
#include "fragmentshader.h"
#include "vertexshader.h"
#include "shaderprogram.h"
#include "renderable.h"
#include "vertexbuffer.h"
#include "png.h"
#include "skinnedobject.h"
#include "texture.h"
#include "vertex.h"
#include "model.h"
#include "resources.h"
#include "obj.h"
#include "rendertarget.h"
#include "renderpass.h"

static const unsigned int screenWidth = 1024;
static const unsigned int screenHeight = 768;

static const char *nullfs =
"#version 330\n"
"\n"
"uniform sampler2D modeltex;\n"
"in vec3 frag_position;\n"
"in vec3 frag_normal;\n"
"in vec2 frag_texcoord;\n"
"out vec4 output[];\n"
"\n"
"void main(void)\n"
"{\n"
"  output[0] = texture2D(modeltex, frag_texcoord);\n"
"  output[1] = vec4((frag_normal + 1.0f) / 2, 1.0f);\n"
"  float depth = 1.0f - (-frag_position.z  / 1000.0f);\n"
"  output[2] = vec4(depth, depth, depth, 1.0f);\n"
"}\n";


static const char *nullvs =
"#version 330\n"
"uniform mat4 projectionMatrix;\n"
"uniform mat4 viewMatrix;\n"
"uniform mat4 modelMatrix;\n"
"\n"
"in vec3 position;\n"
"in vec3 normal;\n"
"in vec2 texcoord;\n"
"out vec3 frag_position;\n"
"out vec3 frag_normal;\n"
"out vec2 frag_texcoord;\n"
"\n"
"void main()\n"
"{\n"
"  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);\n"
"  frag_position = (viewMatrix * modelMatrix * vec4(position, 1.0)).xyz;\n"
"  frag_normal = normal;\n"
"  frag_texcoord = texcoord;\n"
"}\n";


static const char *texturesvs =
"#version 330\n"
"uniform mat4 projectionMatrix;\n"
"uniform mat4 viewMatrix;\n"
"uniform mat4 modelMatrix;\n"
"\n"
"in vec3 position;\n"
"in vec3 normal;\n"
"in vec2 texcoord;\n"
"out vec3 frag_position;\n"
"out vec3 frag_normal;\n"
"out vec2 frag_texcoord;\n"
"\n"
"void main()\n"
"{\n"
"  gl_Position = vec4(position, 1.0);\n"
"  frag_position = gl_Position.xyz;\n"
"  frag_normal = normal;\n"
"  frag_texcoord = texcoord;\n"
"}\n";


class SDLException : public std::exception
{
public:
	SDLException(const std::string &errors)
		: errors(errors)
	{}
	~SDLException() throw () {}
	const char *what() const throw() { return errors.c_str(); }
private:
	const std::string errors;
};
		
static vertex v0[] {
	{ 0, 0, 1,  0,  0, 1,  0,  0 },
	{ 0, 1, 1,  0,  0, 1,  0,  1 },
	{ 1, 0, 1,  0,  0, 1,  1,  0 },

	{ 0, 1, 1,  0,  0, -1,  0,  1 },
	{ 1, 1, 1,  0,  0, -1,  1,  1 },
	{ 1, 0, 1,  0,  0, -1,  1,  0 },

};
static vertex v1[] {
	{ -1, -1, 1,  0,  0, 1,  0,  0 },
	{ -1,  1, 1,  0,  0, 1,  0,  1 },
	{  1, -1, 1,  0,  0, 1,  1,  0 },

	{ -1,  1, 1,  0,  0, -1,  0,  1 },
	{  1,  1, 1,  0,  0, -1,  1,  1 },
	{  1, -1, 1,  0,  0, -1,  1,  0 },

};

class Textures : public Renderable
{
public:
	void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES)
	{
		std::vector<std::shared_ptr<Texture>> textures;
		for(auto it = Resources<Texture>::data.begin(); it != Resources<Texture>::data.end(); it++)
		{
			if(it->second->debug) textures.push_back(it->second);
		}
		if (textures.size() == 0) return;
		size_t width = 0;
		while(width * width < textures.size()) width++;
		float stepsize = 2.0f / width;
		for(size_t i = 0; i < textures.size(); i++)
		{
			float xoffset = -1 + stepsize * (i % width);
			float yoffset = -1 + stepsize * (i / width);
			for(int j = 0; j < 6; j++)
			{
				v1[j].x = v0[j].x * stepsize + xoffset;
				v1[j].y = v0[j].y * stepsize + yoffset;
			}
			VertexBuffer vertexBuffer(modelRefs, v1, sizeof(v1)/sizeof(vertex));
			textures[i]->Bind(shaderProgram, "modeltex");	
			vertexBuffer.Draw(shaderProgram);	
		}
	}
};

class Entity
{
	public:
		Entity(Model &model, glm::vec3 position)
		: model(model)
		, position(position)
		{
		}
		void Update(glm::vec3 position, glm::quat rotation, btTransform trans)
		{
			this->position = position;
			this->rotation = rotation;
			//TODO: remove this
			this->trans = trans;
		}
		void Draw(ShaderProgram& shaderProgram)
		{
			glm::mat4 currentModelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation);
//			shaderProgram.Set("modelMatrix", &currentModelMatrix[0][0]);
			trans.getOpenGLMatrix((float*)&currentModelMatrix);
			shaderProgram.Set("modelMatrix", &currentModelMatrix[0][0]);
			model.Draw(shaderProgram);
		}
	private:
		Model &model;
		glm::vec3 position;
		glm::quat rotation;
		btTransform trans;
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
			throw SDLException("SDL_Init failed\n");
		}

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
		mainwindow = SDL_CreateWindow("zero", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	
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

class DefaultPass : public RenderPass
{
public:
	DefaultPass()
	: RenderPass(Resources<VertexShader>::Load("texturesvs", std::shared_ptr<VertexShader>(new VertexShader(texturesvs))), Resources<FragmentShader>::Load("nullfs", std::shared_ptr<FragmentShader>(new FragmentShader(nullfs))), screenWidth, screenHeight)
	{
	}
};

class DeferredPass : public RenderPass
{
public:
	DeferredPass()
	: RenderPass(Resources<VertexShader>::Load("nullvs", std::shared_ptr<VertexShader>(new VertexShader(nullvs))), Resources<FragmentShader>::Load("nullfs", std::shared_ptr<FragmentShader>(new FragmentShader(nullfs))), screenWidth, screenHeight)
	{
		rendertarget->AddTarget(Resources<Texture>::Load("color", std::shared_ptr<Texture>(new Texture(screenWidth, screenHeight, true))));
		rendertarget->AddTarget(Resources<Texture>::Load("normal", std::shared_ptr<Texture>(new Texture(screenWidth, screenHeight, true))));
		rendertarget->AddTarget(Resources<Texture>::Load("depth", std::shared_ptr<Texture>(new Texture(screenWidth, screenHeight, true))));
	}
};

class SceneGraph : public Renderable 
{
	public:
		SceneGraph(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
		: projectionMatrix(projectionMatrix)
		, viewMatrix(viewMatrix)
		{
		}
		void SetViewMatrix(glm::mat4 viewMatrix)
		{
			this->viewMatrix = viewMatrix;
		}
		void Add(std::shared_ptr<Entity> entity)
		{
			graph.push_back(entity);
		}
		void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES)
		{
			shaderProgram.Set("viewMatrix", &viewMatrix[0][0]);
			shaderProgram.Set("projectionMatrix", &projectionMatrix[0][0]);
			for(size_t i=0; i< graph.size(); i++)
			{
				graph[i]->Draw(shaderProgram);
			}
		}
	private:
		std::vector<std::shared_ptr<Entity>> graph;
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
};

class Camera
{
public:
	Camera()
	: quit(false)
	, projectionmatrix(glm::perspective(60.0f, (float)screenWidth / (float)screenHeight, 1.0f, 10000.0f))
	, viewmatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)))
	, position(glm::vec3(0.0f, 0.0f, 4.0f))
	, up(false), down(false), left(false), right(false)
	, w(false), a(false), s(false), d(false)
	, speed(0.1)
	{
	}
	void Update()
	{
		SDL_Event keyevent; 
		SDL_PollEvent(&keyevent);
		if (keyevent.type == SDL_KEYDOWN)
		{
			switch(keyevent.key.keysym.sym)
			{
				case SDLK_LEFT:
					left = true;
				break;
				case SDLK_RIGHT:
					right = true;
				break;
				case SDLK_UP:
					up = true;
				break;
				case SDLK_DOWN:
					down = true;
				break;
				case SDLK_w:
					w = true;
				break;
				case SDLK_a:
					a = true;
				break;
				case SDLK_s:
					s = true;
				break;
				case SDLK_d:
					d = true;
				break;
				case SDLK_ESCAPE:
					quit = true;
				break;
				default:
				break;
			}
		}
		if (keyevent.type == SDL_KEYUP)
		{
			switch(keyevent.key.keysym.sym)
			{
				case SDLK_LEFT:
					left = false;
				break;
				case SDLK_RIGHT:
					right = false;
				break;
				case SDLK_UP:
					up = false;
				break;
				case SDLK_DOWN:
					down = false;
				break;
				case SDLK_w:
					w = false;
				break;
				case SDLK_a:
					a = false;
				break;
				case SDLK_s:
					s = false;
				break;
				case SDLK_d:
					d = false;
				break;
				default:
				break;
			}
		}
		Recalculate();
	}

	void Recalculate()
	{
		if(left) MoveX(-10 * speed);
		if(right) MoveX(10 * speed);
		if(up) MoveZ(10 * speed);
		if(down) MoveZ(-10 * speed);
		if(w) RotateX(5.0 * speed);
		if(s) RotateX(-5.0 * speed);
		if(a) RotateY(5.0 * speed);
		if(d) RotateY(-5.0 * speed);
	}
	
	glm::mat4 GetViewMatrix()
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
	glm::mat4 projectionmatrix;
	glm::mat4 viewmatrix;
private:
	glm::vec3 position;
	glm::quat orientation;
	bool up, down, left, right;
	bool w, a, s, d;
	float speed;
};

	
class Window
{
public:
	Window()
	: window()
	, entity(new Entity(Model::Cube(), glm::vec3(0.0f, 50.0f, 0.0f)))
//	, sponza(Obj::ObjParser("data", "sponzatri.obj").GetModel())
	, sceneGraph(camera.projectionmatrix, camera.viewmatrix)
	{
		sceneGraph.Add(entity);
		sceneGraph.Add(std::shared_ptr<Entity>(new Entity(Model::Square(), glm::vec3(0.0f, 0.0f, 0.0f))));
//		sceneGraph.Add(new Entity(*sponza, glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, -60.0f))));

 
        btBroadphaseInterface* broadphase = new btDbvtBroadphase();
        btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
        btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
 
        btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
 
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
 
        dynamicsWorld->setGravity(btVector3(0,-10,0));
 
 
        btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
 
        btCollisionShape* fallShape = new btSphereShape(1);
 
 
        btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
        btRigidBody::btRigidBodyConstructionInfo
                groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
        btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
        dynamicsWorld->addRigidBody(groundRigidBody);
 
 
        btDefaultMotionState* fallMotionState =
                new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,50,0)));
        btScalar mass = 1;
        btVector3 fallInertia(0,0,0);
        fallShape->calculateLocalInertia(mass,fallInertia);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
        fallRigidBody = new btRigidBody(fallRigidBodyCI);
        dynamicsWorld->addRigidBody(fallRigidBody);
 
 
/*        dynamicsWorld->removeRigidBody(fallRigidBody);
        delete fallRigidBody->getMotionState();
        delete fallRigidBody;
 
        dynamicsWorld->removeRigidBody(groundRigidBody);
        delete groundRigidBody->getMotionState();
        delete groundRigidBody;
 
 
        delete fallShape;
 
        delete groundShape;
 
 
        delete dynamicsWorld;
        delete solver;
        delete collisionConfiguration;
        delete dispatcher;
        delete broadphase;
*/
	}
	~Window()
	{
	}
	void Render()
	{
                dynamicsWorld->stepSimulation(1/60.f,10);
                btTransform trans;
                fallRigidBody->getMotionState()->getWorldTransform(trans);
		glm::quat q;
		entity->Update(glm::vec3(1.0f, 1.0f, 1.0f), q, trans);
		
		sceneGraph.SetViewMatrix(camera.GetViewMatrix());
		deferredpass.Render(sceneGraph);
		defaultpass.Render(debugtextures);
		window.Swap();
	}
public:
	Camera camera;
private:
	GLWindow window;
//	std::unique_ptr<Model> sponza;
	std::shared_ptr<Entity> entity;
	SceneGraph sceneGraph;
	DeferredPass deferredpass;
	DefaultPass defaultpass;
	Textures debugtextures;
	btRigidBody* fallRigidBody;
	btDiscreteDynamicsWorld* dynamicsWorld;
};

int main()
{
	try
	{
		Window window;
		int frameCounter = 0;
		while(!window.camera.quit)
		{
			window.camera.Update();
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

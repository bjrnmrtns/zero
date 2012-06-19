#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <exception>
// General comment. idsoftwares coordinate system
// X -> Left/Right, Y -> Forward/Backward, Z -> Up/Down
class Tokenizer
{
class exception : public std::exception
{
public:
	exception(const std::string &errors)
	: errors(errors)
	{
	}
	~exception() throw ()
	{
	}
	const char *what() const throw()
	{
		return errors.c_str();
	}
private:
	const std::string errors;
};
private:
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	boost::char_separator<char> delimiters;
	std::ifstream file;
	std::string currentline;
	std::string current;
	tokenizer tokens;
	tokenizer::iterator tok_iter;
public:
	Tokenizer(const std::string filename, const std::string whitespace, const std::string delimiters)
	: delimiters(whitespace.c_str(), delimiters.c_str())
	, file(filename)
	, tokens(currentline, this->delimiters)
	, tok_iter(tokens.end())
	{
	}
	bool good()
	{
		return file.good();
	}
	std::string next()
	{
		while(tok_iter == tokens.end())
		{
			if(!file.good()) 
			{
				current = "";
				return current;
			}
			std::getline(file, currentline);
			currentline = currentline.substr(0, currentline.find("//"));
			tokens.assign(currentline, this->delimiters);
			tok_iter = tokens.begin();
		}
		current = *tok_iter;
		++tok_iter;
		return current;
	}
	std::string token()
	{
		return current;
	}
	void expectnext(std::string expected)
	{
		check(expected, next());
	}
	void expectcurrent(std::string expected)
	{
		check(expected, current);
	}
	void check(std::string expected, std::string actual)
	{
		if(actual != expected) throw exception("expected: " + expected + " instead: " + actual);
	}
};

namespace md5
{
	class exception : public std::exception
	{
	public:
		exception(const std::string &errors)
		: errors(errors)
		{
		}
		~exception() throw ()
		{
		}
		const char *what() const throw()
		{
			return errors.c_str();
		}
	private:
		const std::string errors;
	};
	static char whitespace[] = " \t\r\n";
	static char delimiters[] = "{}()";
	struct joint
	{
		std::string name;
		int parentid;
		// flags only used for animation.
		unsigned int flags;
		glm::vec3 pos;
		glm::quat orient;
		// index only used for animation.
		unsigned int index;
	};
	struct vertex
	{
		unsigned int index;
		glm::vec2 texcoord;
		unsigned int wid;
		unsigned int wcount;
	};
	struct triangle
	{
		unsigned int index;
		unsigned int v0, v1, v2;
	};
	struct weight
	{
		unsigned int index;
		unsigned int jointid;
		float bias;
		glm::vec3 pos;
	};
	struct mesh
	{
		std::string shader;
		unsigned int vertcount;
		std::vector<vertex> vertices;
		unsigned int trianglecount;
		std::vector<triangle> triangles;
		unsigned int weightcount;
		std::vector<weight> weights;

	};
	struct model
	{
		std::string version;
		std::string commandline;
		unsigned int jointcount;
		unsigned int meshcount;
		std::vector<joint> joints;
		std::vector<mesh> meshes;
	};
	static void calcwcomp(glm::quat& q)
	{
		float t = 1.0f - (q.x * q.x) - (q.y * q.y) - (q.z * q.z);
		if(t < 0.0f) q.w = 0.0f;
		else q.w = -std::sqrt(t);
	}
	class meshfile
	{
	private:
		static void checkcount(std::string name, size_t expected, size_t actual)
		{
			if(expected != actual)
			{
				std::stringstream sexp, sact;
				sexp << expected;
				sact << actual;
				throw exception(name + " expected: " + sexp.str() + " actual: " + sact.str());
			}
		}
	public:
		static void parse(Tokenizer& tokenizer, model& m)
		{
			if(!tokenizer.good()) throw exception("failed to parse");
			tokenizer.expectnext("MD5Version");
			m.version = tokenizer.next();
			tokenizer.expectnext("commandline");
			m.commandline = tokenizer.next();
			tokenizer.expectnext("numJoints");
			m.jointcount = boost::lexical_cast<unsigned int>(tokenizer.next());
			tokenizer.expectnext("numMeshes");
			m.meshcount = boost::lexical_cast<unsigned int>(tokenizer.next());
			
			while(tokenizer.good())
			{
				tokenizer.next();
				if(tokenizer.token() == "joints")
				{
					tokenizer.expectnext("{");
					parsejoints(tokenizer, m);
					checkcount("joints", m.jointcount, m.joints.size());
				} 
				else if(tokenizer.token() == "mesh")
				{
					tokenizer.expectnext("{");
					parsemesh(tokenizer, m);
					checkcount("mesh vertices", m.meshes.back().vertcount, m.meshes.back().vertices.size());
					checkcount("mesh triangles", m.meshes.back().trianglecount, m.meshes.back().triangles.size());
					checkcount("mesh weights", m.meshes.back().weightcount, m.meshes.back().weights.size());
				}
			}
			checkcount("meshes", m.meshcount, m.meshes.size());
		}
		static void parsemesh(Tokenizer& tok, model& model)
		{
			mesh tmp;
			model.meshes.push_back(tmp);
			mesh& m = model.meshes.back();
			tok.expectnext("shader");
			m.shader = tok.next();
			tok.expectnext("numverts");
			m.vertcount = boost::lexical_cast<unsigned int>(tok.next());
			for(size_t i = 0; i < m.vertcount; i++)
			{
				m.vertices.push_back(parsevertex(tok));
			}
			tok.expectnext("numtris");
			m.trianglecount = boost::lexical_cast<unsigned int>(tok.next());
			for(size_t i = 0; i < m.trianglecount; i++)
			{
				m.triangles.push_back(parsetriangle(tok));
			}
			tok.expectnext("numweights");
			m.weightcount = boost::lexical_cast<unsigned int>(tok.next());
			for(size_t i = 0; i < m.weightcount; i++)
			{
				m.weights.push_back(parseweight(tok));
			}
			tok.expectnext("}");
		}
		static vertex parsevertex(Tokenizer& tok)
		{
			vertex v;
			tok.expectnext("vert");
			v.index = boost::lexical_cast<unsigned int>(tok.next());
			tok.expectnext("(");
			v.texcoord.x = 1.0f - boost::lexical_cast<float>(tok.next());
			v.texcoord.y = 1.0f - boost::lexical_cast<float>(tok.next());
			tok.expectnext(")");
			v.wid = boost::lexical_cast<unsigned int>(tok.next());
			v.wcount = boost::lexical_cast<unsigned int>(tok.next());
			return v;
		}
		static triangle parsetriangle(Tokenizer& tok)
		{
			triangle t;
			tok.expectnext("tri");
			t.index = boost::lexical_cast<unsigned int>(tok.next());
			t.v0 = boost::lexical_cast<unsigned int>(tok.next());
			t.v1 = boost::lexical_cast<unsigned int>(tok.next());
			t.v2 = boost::lexical_cast<unsigned int>(tok.next());
			return t;
		}
		static weight parseweight(Tokenizer& tok)
		{
			weight w;
			tok.expectnext("weight");
			w.index = boost::lexical_cast<unsigned int>(tok.next());
			w.jointid = boost::lexical_cast<unsigned int>(tok.next());
			w.bias = boost::lexical_cast<float>(tok.next());
			tok.expectnext("(");
			w.pos.x = boost::lexical_cast<float>(tok.next());
			w.pos.z = -boost::lexical_cast<float>(tok.next());
			w.pos.y = boost::lexical_cast<float>(tok.next());
			tok.expectnext(")");
			return w;
		}
		static glm::vec3 getfinalpos(const vertex& v, const mesh& m, const std::vector<joint>& joints)
		{
			glm::vec3 final;
			for(size_t i = v.wid; i < v.wid + v.wcount; i++)
			{
				const weight& w = m.weights[i];
				const joint& j = joints[w.jointid];
				glm::vec3 wv = j.orient * w.pos;
				final += (j.pos + wv) * w.bias;
			}
			return final;
		}
		static void parsejoints(Tokenizer& tok, model& m)
		{
			while(tok.next() != "}")
			{
				joint j;
				j.name = tok.token();
				j.parentid = boost::lexical_cast<int>(tok.next());
				tok.expectnext("(");
				j.pos.x = boost::lexical_cast<float>(tok.next());
				j.pos.z = -boost::lexical_cast<float>(tok.next());
				j.pos.y = boost::lexical_cast<float>(tok.next());
				tok.expectnext(")");
				tok.expectnext("(");
				j.orient.x = boost::lexical_cast<float>(tok.next());
				j.orient.z = -boost::lexical_cast<float>(tok.next());
				j.orient.y = boost::lexical_cast<float>(tok.next());
				calcwcomp(j.orient);
				tok.expectnext(")");
				m.joints.push_back(j);
			}
		}
	};
	struct bound
	{
		glm::vec3 min;
		glm::vec3 max;
	};
	struct baseframeval
	{
		glm::vec3 pos;
		glm::quat orient;
	};
	struct frame
	{
		std::vector<float> diffs;
	};
	struct anim
	{
		std::string version;
		std::string commandline;
		unsigned int framecount;
		unsigned int jointcount;
		unsigned int framerate;
		unsigned int animcompcount;
		std::vector<joint> joints;
		std::vector<bound> bounds;
		std::vector<baseframeval> baseframe;
		std::vector<frame> frames;
	};
	class animfile
	{
	private:
		static void checkcount(std::string name, size_t expected, size_t actual)
		{
			if(expected != actual)
			{
				std::stringstream sexp, sact;
				sexp << expected;
				sact << actual;
				throw exception(name + " expected: " + sexp.str() + " actual: " + sact.str());
			}
		}
	public:
		static void parse(Tokenizer& tokenizer, anim& a)
		{
			unsigned int framecounter = 0;
			if(!tokenizer.good()) throw exception("failed to parse");
			tokenizer.expectnext("MD5Version");
			a.version = tokenizer.next();
			tokenizer.expectnext("commandline");
			a.commandline = tokenizer.next();
			tokenizer.expectnext("numFrames");
			a.framecount = boost::lexical_cast<unsigned int>(tokenizer.next());
			tokenizer.expectnext("numJoints");
			a.jointcount = boost::lexical_cast<unsigned int>(tokenizer.next());
			tokenizer.expectnext("frameRate");
			a.framerate = boost::lexical_cast<unsigned int>(tokenizer.next());
			tokenizer.expectnext("numAnimatedComponents");
			a.animcompcount = boost::lexical_cast<unsigned int>(tokenizer.next());
			while(tokenizer.good())
			{
				tokenizer.next();
				if(tokenizer.token() == "hierarchy")
				{
					tokenizer.expectnext("{");
					parsehierarchy(tokenizer, a);
					checkcount("hierarchy", a.jointcount, a.joints.size());
				}
				else if(tokenizer.token() == "bounds")
				{
					tokenizer.expectnext("{");
					parsebounds(tokenizer, a);
					checkcount("bounds", a.framecount, a.bounds.size());
				}
				else if(tokenizer.token() == "baseframe")
				{
					tokenizer.expectnext("{");
					parsebaseframe(tokenizer, a);
					checkcount("baseframe", a.jointcount, a.baseframe.size());
				}
				else if(tokenizer.token() == "frame")
				{
					unsigned int framenumber = boost::lexical_cast<unsigned int>(tokenizer.next());
					checkcount("framenumber", framenumber, framecounter++);
					tokenizer.expectnext("{");
					parseframe(tokenizer, a);
					checkcount("frame", a.animcompcount, a.frames.back().diffs.size());
				}
			}
		}
		static void parseframe(Tokenizer& tok, anim& a)
		{
			frame f;
			while(tok.next() != "}")
			{
				f.diffs.push_back(boost::lexical_cast<float>(tok.token()));
			}
			a.frames.push_back(f);
		}
		static void parsebaseframe(Tokenizer& tok, anim& a)
		{
			while(tok.next() != "}")
			{
				baseframeval b;
				tok.expectcurrent("(");
				b.pos.x = boost::lexical_cast<float>(tok.next());
				b.pos.z = -boost::lexical_cast<float>(tok.next());
				b.pos.y = boost::lexical_cast<float>(tok.next());
				tok.expectnext(")");
				tok.expectnext("(");
				b.orient.x = boost::lexical_cast<float>(tok.next());
				b.orient.z = -boost::lexical_cast<float>(tok.next());
				b.orient.y = boost::lexical_cast<float>(tok.next());
				calcwcomp(b.orient);
				tok.expectnext(")");
				a.baseframe.push_back(b);
			}
		}
		static void parsebounds(Tokenizer& tok, anim& a)
		{
			while(tok.next() != "}")
			{
				bound b;
				tok.expectcurrent("(");
				b.min.x = boost::lexical_cast<float>(tok.next());
				b.min.z = -boost::lexical_cast<float>(tok.next());
				b.min.y = boost::lexical_cast<float>(tok.next());
				tok.expectnext(")");
				tok.expectnext("(");
				b.max.x = boost::lexical_cast<float>(tok.next());
				b.max.z = -boost::lexical_cast<float>(tok.next());
				b.max.y = boost::lexical_cast<float>(tok.next());
				tok.expectnext(")");
				a.bounds.push_back(b);
			}
		}
		static void parsehierarchy(Tokenizer& tok, anim& a)
		{
			while(tok.next() != "}")
			{
				joint j;
				j.name = tok.token();
				j.parentid = boost::lexical_cast<int>(tok.next());
				j.flags = boost::lexical_cast<unsigned int>(tok.next());
				j.index = boost::lexical_cast<unsigned int>(tok.next());
				a.joints.push_back(j);
			}
		}
	};
}

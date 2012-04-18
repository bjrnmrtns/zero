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
	void expectnext(std::string token)
	{
		if(next() != token) throw exception("expected: " + token);
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
		glm::vec3 pos;
		glm::quat orient;
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
			v.texcoord.x = boost::lexical_cast<float>(tok.next());
			v.texcoord.y = boost::lexical_cast<float>(tok.next());
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
			w.pos.y = boost::lexical_cast<float>(tok.next());
			w.pos.z = boost::lexical_cast<float>(tok.next());
			tok.expectnext(")");
			return w;
		}
		static glm::vec3 getfinalpos(vertex& v, model& mod, mesh& m)
		{
			glm::vec3 final;
			for(size_t i = v.wid; i < v.wid + v.wcount; i++)
			{
				weight& w = m.weights[i];
				joint& j = mod.joints[w.jointid];
				glm::vec3 wv = j.orient * w.pos;
				final += (j.pos + wv) * w.bias;
			}
			return final;
		}
		static void calcwcomp(glm::quat& q)
		{
			float t = 1.0f - (q.x * q.x) - (q.y * q.y) - (q.z * q.z);
			if(t < 0.0f) q.w = 0.0f;
			else q.w = -std::sqrt(t);
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
				j.pos.y = boost::lexical_cast<float>(tok.next());
				j.pos.z = boost::lexical_cast<float>(tok.next());
				tok.expectnext(")");
				tok.expectnext("(");
				j.orient.x = boost::lexical_cast<float>(tok.next());
				j.orient.y = boost::lexical_cast<float>(tok.next());
				j.orient.z = boost::lexical_cast<float>(tok.next());
				calcwcomp(j.orient);
				tok.expectnext(")");
				m.joints.push_back(j);
			}
		}
	};
}

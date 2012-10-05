#ifndef SHADER
#define SHADER

class Shader
{
public:
	Shader(const char* source, int type);
	virtual ~Shader();
	int GetId();
private:
	int id;
	int type;
};

#endif

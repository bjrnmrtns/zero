class ShaderProgram
{
public:
	void use() {};
};

class Renderable
{
public:
	void draw(const ShaderProgram& sp) {};
};

class RenderTarget
{
public:
	void use() {};
};

int main()
{
	RenderTarget deferredtarget;
	Renderable renderable;
	ShaderProgram shaderprogram;

	deferredtarget.use();
	renderable.draw(shaderprogram);
}

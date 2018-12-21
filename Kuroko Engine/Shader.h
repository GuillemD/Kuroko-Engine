#ifndef _SHADER_H_
#define _SHADER_H_

#include "Globals.h"

enum ShaderType{ UNKNOWN, VERTEX, FRAGMENT, TESSELLATION, GEOMETRY};

class Shader {
public:
	Shader(ShaderType type);
	~Shader();

	void CreateVertexShader(const char* ShaderSource);
	void CreateFragmentShader(const char* ShaderSource);

	uint getId() const { return id; };
	ShaderType getType() const { return type; };

	void SetSourceCode(const char* code);

	bool compile_success = false;

	char* GetSourceCode() { return source; }
private:
	
	ShaderType type = UNKNOWN;
	uint id = 0;
	char* source = nullptr;

	
};

class ShaderProgram
{
public:
	ShaderProgram(Shader* vertex, Shader* fragment);
	~ShaderProgram();

	void CreateProgram();
	void UseProgram();

	void SetVertexShader(Shader* _vs);
	void SetFragmentShader(Shader* _fs);

	Shader* GetVertexShader() const { return vs; };
	Shader* GetFragmentShader() const { return fs; };

	uint getProgramId() const { return id; };

private:
	Shader* vs = nullptr;
	Shader* fs = nullptr;

	uint id = 0;
	bool link_success = false;

};

#endif // !_SHADER_H_

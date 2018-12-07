#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include "Globals.h"

class Shader;

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

	uint getId() const { return id; };

private:
	Shader* vs = nullptr;
	Shader* fs = nullptr;

	uint id = 0;
	bool link_success = false;
	
};

#endif // !_SHADER_PROGRAM_H_

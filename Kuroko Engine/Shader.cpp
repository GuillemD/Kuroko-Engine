#include "Shader.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "Applog.h"
#include "ModuleImporter.h"
#include "ModuleResourcesManager.h"


Shader::Shader(ShaderType type) : type(type)
{

}

Shader::~Shader()
{
}

uint Shader::CreateVertexShader(const char* ShaderSource)
{
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &ShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		//LOG("Shader compilation error: %s", infoLog);
		glDeleteShader(vertexShader);
		compile_success = false;
		return 0;

	}
	compile_success = true;
	return vertexShader;
}

uint Shader::CreateFragmentShader(const char* ShaderSource)
{
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &ShaderSource, NULL);
	glCompileShader(fragmentShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		//LOG("Shader compilation error: %s", infoLog);
		glDeleteShader(fragmentShader);
		compile_success = false;
		return 0;

	}
	compile_success = true;
	return fragmentShader;
}


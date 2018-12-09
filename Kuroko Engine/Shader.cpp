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
	source = new char[2048];

}

Shader::~Shader()
{
}

void Shader::CreateVertexShader(const char* ShaderSource)
{
	//SET SOURCE WITH CODE PROVIDED
	SetSourceCode(ShaderSource);

	//CREATE SHADER
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
		app_log->AddLog("Shader compilation error: %s", infoLog);
		glDeleteShader(vertexShader);
		compile_success = false;
		id = 0;

	}
	compile_success = true;
	id = vertexShader;
}

void Shader::CreateFragmentShader(const char* ShaderSource)
{
	//SET SOURCE WITH CODE PROVIDED
	SetSourceCode(ShaderSource);

	//CREATE SHADER
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
		app_log->AddLog("Shader compilation error: %s", infoLog);
		glDeleteShader(fragmentShader);
		compile_success = false;
		id = 0;

	}
	compile_success = true;
	id = fragmentShader;
}

void Shader::SetSourceCode(const char * code)
{
	strcpy_s(source, 2048, code);
}

ShaderProgram::ShaderProgram(Shader* vertex, Shader* fragment) : vs(vertex), fs(fragment)
{
	CreateProgram();
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::CreateProgram()
{
	// Get a program object.
	GLuint program = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(program, vs->getId());
	glAttachShader(program, fs->getId());

	// Link our program
	glLinkProgram(program);

	GLint isLinked = 0;
	GLchar infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		app_log->AddLog("Program linking error: %s", infoLog);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(vs->getId());
		glDeleteShader(fs->getId());

		link_success = false;
		id = 0;
	}
	link_success = true;
	id = program;
	// Always detach shaders after a successful link.
	glDetachShader(program, vs->getId());
	glDetachShader(program, fs->getId());
}

void ShaderProgram::UseProgram()
{
	glUseProgram(id);
}

void ShaderProgram::SetVertexShader(Shader* _vs)
{
	vs = _vs;
}

void ShaderProgram::SetFragmentShader(Shader* _fs)
{
	fs = _fs;
}


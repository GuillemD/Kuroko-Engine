#include "ShaderProgram.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "Applog.h"
#include "ModuleImporter.h"
#include "ModuleResourcesManager.h"
#include "Shader.h"

ShaderProgram::ShaderProgram(Shader* vertex, Shader* fragment): vs(vertex), fs(fragment)
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

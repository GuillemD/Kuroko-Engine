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

	switch (type)
	{
	case VERTEX:
	{
		strcpy_s(source, 2048, vertex_default);
		CreateVertexShader(source);
		if (compile_success)
		{
			//UpdateShader();
		}
	}
	break;
	case FRAGMENT:
	{
		strcpy_s(source, 2048, fragment_default);
		CreateFragmentShader(source);
		if (compile_success)
		{
			//UpdateShader();
		}
	}
	break;
	default:
		break;
	}

}

Shader::~Shader()
{
}

void Shader::CreateVertexShader(const char* ShaderSource)
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


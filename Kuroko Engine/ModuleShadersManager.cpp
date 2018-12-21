#include "ModuleShadersManager.h"
#include "Shader.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "Applog.h"
#include "ModuleImporter.h"
#include "ModuleResourcesManager.h"
#include "Resource.h"


ModuleShadersManager::ModuleShadersManager(Application * app, bool start_enabled) : Module(app, start_enabled)
{
	name = "shaders manager";
}

ModuleShadersManager::~ModuleShadersManager()
{
}

bool ModuleShadersManager::Init(const JSON_Object * config)
{
	return true;
}

bool ModuleShadersManager::Start()
{
	bool ret = true;

	SetDefaultShaders();
	SetDefaultProgram();

	return ret;
}

bool ModuleShadersManager::CleanUp()
{
	/*for (auto it = shaders.begin(); it != shaders.end(); it++) {

		if ((*it) != NULL && (*it) != nullptr)
		{
			delete (*it);
			(*it) = nullptr;
			it = shaders.erase(it);
		}

	}


	for (auto it = programs.begin(); it != programs.end(); it++) {

		if ((*it) != NULL && (*it) != nullptr)
		{
			delete (*it);
			(*it) = nullptr;
			it = programs.erase(it);
		}

	}*/
	return true;
}


ShaderProgram * ModuleShadersManager::FindShaderProgram(Shader * vert_shader, Shader * frag_shader)
{
	for (int i = 0; i < programs.size(); ++i)
	{
		if (programs[i]->GetVertexShader() == vert_shader  && programs[i]->GetFragmentShader() == frag_shader)
		{
			return programs[i];
		}
	}


	return nullptr;
}

ShaderProgram * ModuleShadersManager::FindShaderProgramByUniqueId(uint shaderuid)
{
	for (int i = 0; i < programs.size(); ++i)
	{
		if (programs[i]->getId() == shaderuid)
		{
			return programs[i];
		}
	}

	return nullptr;
}

Shader * ModuleShadersManager::FindShaderByUniqueId(uint shaderuid)
{
	for (int i = 0; i < shaders.size(); ++i)
	{
		if (shaders[i]->getId() == shaderuid)
		{
			return shaders[i];
		}
	}

	return nullptr;
}






void ModuleShadersManager::SetDefaultShaders()
{

	//DEFAULT VERTEX SHADER CODE
	char vertex_default[512] =

		"#version 330 core\n\
	layout(location = 0) in vec3 position;\n\
	layout(location = 1) in vec3 normal;\n\
	layout(location = 2) in vec3 color;\n\
	layout(location = 3) in vec2 texCoord;\n\\n\
	out vec4 ourColor;\n\
	out vec2 TexCoord;\n\
	uniform mat4 model_matrix;\n\
	uniform mat4 view_matrix;\n\
	uniform mat4 projection_matrix;\n\
	void main()\n\
	{\n\\n\
		gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0f);\n\
		ourColor = vec4(color, 1.0f);\n\
		TexCoord = texCoord;\n\
	}";


	default_vertex_shader = new Shader(VERTEX);
	default_vertex_shader->CreateVertexShader(vertex_default);
	if (default_vertex_shader->getId() != 0)
		shaders.push_back(default_vertex_shader);





	//DEFAULT FRAGMENT SHADER CODE
	char fragment_default[512] =
		"#version 330 core\n\
	in vec4 ourColor;\n\
	in vec2 TexCoord;\n\\n\
	out vec4 color;\n\
	uniform sampler2D ourTexture;\n\
	void main()\n\
	{\n\\n\
		color = texture(ourTexture, TexCoord);\n\
	}";

	default_fragment_shader = new Shader(FRAGMENT);
	default_fragment_shader->CreateVertexShader(fragment_default);
	if (default_fragment_shader->getId() != 0)
		shaders.push_back(default_fragment_shader);




}

void ModuleShadersManager::SetDefaultProgram()
{
	default_shader_program = new ShaderProgram(default_vertex_shader, default_fragment_shader);

	programs.push_back(default_shader_program);
}

bool ModuleShadersManager::ImportShader(const char * file_original_name, std::string file_binary_name, std::string extension)
{
	bool ret = true;

	if (extension == ".vert")
	{
		Shader* shader = new Shader(VERTEX);
	}
	else if (extension == ".frag")
	{
		Shader* shader = new Shader(FRAGMENT);
	}
	//set code + compile it
	//save to library
	//delete shader

	return ret;
}

Shader * ModuleShadersManager::LoadShaderFromLibrary(const char * file)
{
	return nullptr;
}

void ModuleShadersManager::AddShaderProgram(ShaderProgram* program) {

	programs.push_back(program);

}

#include "ModuleShadersManager.h"
#include "Shader.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "Applog.h"
#include "ModuleImporter.h"
#include "ModuleResourcesManager.h"


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
	
	return true;
}


void ModuleShadersManager::SetDefaultShaders()
{
	//DEFAULT VERTEX SHADER CODE
	char vertex_default[512] =

		"#version 330 core\n\
	layout(location = 0) in vec3 position;\n\
	layout(location = 1) in vec3 color;\n\
	layout(location = 2) in vec2 texCoord;\n\\n\
	out vec3 ourColor;\n\
	out vec2 TexCoord;\n\
	uniform mat4 model;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	void main()\n\
	{\n\\n\
		gl_Position = projection * view * model * vec4(position, 1.0f);\n\
		ourColor = color;\n\
		TexCoord = texCoord;\n\
	}";

	default_vertex_shader = new Shader(VERTEX);
	default_vertex_shader->CreateVertexShader(vertex_default);
	if (default_vertex_shader->getId() != 0)
		shaders.push_back(default_vertex_shader);



	//DEFAULT FRAGMENT SHADER CODE
	char fragment_default[512] =
		"#version 330 core\n\
	in vec3 ourColor;\n\
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

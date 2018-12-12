#include "ResourceShader.h"
#include "Shader.h"
#include "Application.h"
#include "ModuleShadersManager.h"
#include "Applog.h"

ResourceShader::ResourceShader(resource_deff deff) : Resource(deff)
{
}

ResourceShader::~ResourceShader()
{
}

void ResourceShader::LoadToMemory()
{
	shader = App->shaders->LoadShaderFromLibrary(binary.c_str());

	if (!shader->compile_success)
		app_log->AddLog("Error loading binary %s", binary.c_str());
	else
	{
		loaded_in_memory = true;
	}
		
}

void ResourceShader::UnloadFromMemory()
{
	if (!shader)
		return;
	delete shader;
	shader = nullptr;
	loaded_in_memory = false;
}

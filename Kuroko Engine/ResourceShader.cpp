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
	//shader = App->shaders->Loadfromlibrary();

	if (!shader->compile_success)
		app_log->AddLog("Error loading binary %s", binary.c_str());
	else
	{
		loaded_in_memory = true;
	}
		
}

void ResourceShader::UnloadFromMemory()
{
}

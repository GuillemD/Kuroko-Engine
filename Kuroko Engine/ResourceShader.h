#pragma once
#include "Resource.h"
class Shader;

class ResourceShader :
	public Resource {
public:
	ResourceShader(resource_deff deff);
	~ResourceShader();
	void LoadToMemory();
	void UnloadFromMemory();
public:
	Shader* shader;
};

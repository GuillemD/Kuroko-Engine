#ifndef _SHADERS_MANAGER_H_
#define _SHADERS_MANAGER_H_

#include "Module.h"
#include "Shader.h"
#include <vector>


class Shader;
class ShaderProgram;
class Resource;

class ModuleShadersManager : public Module
{
public:
	ModuleShadersManager(Application* app, bool start_enabled = true);
	~ModuleShadersManager();

	bool Init(const JSON_Object* config);
	bool Start();						  
	bool CleanUp();


	Shader* GetDefaultVertex() const { return default_vertex_shader; };
	Shader* GetDefaultFragment() const { return default_fragment_shader; };
	ShaderProgram* GetDefaultProgram() const { return default_shader_program; };

	ShaderProgram* FindShaderProgram(Shader* vert_shader, Shader* frag_shader);
	ShaderProgram* FindShaderProgramByUniqueId(uint shaderuid);
	Shader* FindShaderByUniqueId(uint shaderuid);


	void SetDefaultShaders();
	void SetDefaultProgram();

	bool ImportShader(const char * file_original_name, std::string file_binary_name, std::string extension);
	Shader* LoadShaderFromLibrary(const char * file);

	void AddShaderProgram(ShaderProgram* shader);
private:
	
	std::vector<Shader*> shaders;
	std::vector<ShaderProgram*> programs;
	Shader* default_vertex_shader = nullptr;
	Shader* default_fragment_shader = nullptr;
	ShaderProgram* default_shader_program = nullptr;
};

#endif // !_SHADERS_MANAGER_H_

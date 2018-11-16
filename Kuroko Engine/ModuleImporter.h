#ifndef __MODULE_IMPORTER
#define __MODULE_IMPORTER
#include "Module.h"
#include "Material.h"
#include <vector>

class GameObject;
class ComponentMesh;
class aiNode;
class aiScene;
class AudioFile;
class Mesh;

//enum ImportType { I_NONE, I_GOBJ, I_TEXTURE, I_FX, I_MUSIC};

struct material_resource_deff {
	uint resource_uuid_diffuse = 0;
	uint resource_uuid_ambient = 0;
	uint resource_uuid_normals = 0;
	uint resource_uuid_lightmap = 0;
};

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init(const JSON_Object* config);
	bool CleanUp();

	void* ImportTexturePointer(const char* file);

	// This functions will eventually replace "Import", when resource manager is working
	bool ImportTexture(const char* file_original_name, std::string file_binary_name); // Retruns true on successful import and false if it wasn't
	bool ImportScene(const char* file_original_name, std::string file_binary_name);	  // Not gonna handle materials for now, only meshes.
	//Own file format
	void ExportMeshToKR(const char* file, Mesh* mesh);
	void ExportTextureToDDS(const char* file);
	Mesh* ImportMeshFromKR(const char* file); // Code crashes when importing mesh from .kr without importing something first
	Texture* LoadTextureFromLibrary(const char* file);

private:
	void ImportNodeToSceneRecursive(const aiNode& node, const aiScene& scene, JSON_Value* json_scene, const std::vector<material_resource_deff>& in_mat_id, uint parent = 0);  // TODO: Add a parameter for mat id, a vector of JSON_Objects*
	void ImportMaterialsFromNode(const aiScene& scene, std::vector<material_resource_deff>& out_mat_id);

};
void logAssimp(const char* message, char* user);
#endif
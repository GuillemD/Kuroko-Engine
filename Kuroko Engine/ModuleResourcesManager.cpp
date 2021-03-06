#include "ModuleResourcesManager.h"
#include "FileSystem.h"
#include "Application.h"
#include "Random.h"
#include "ModuleImporter.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "Applog.h"
#include "Mesh.h"

// Temporal debug purposes
#include "ModuleInput.h"
#include <experimental/filesystem>
#include <iostream>

ModuleResourcesManager::ModuleResourcesManager(Application* app, bool start_enabled): Module(app, start_enabled)
{
	name = "resource manager";
}


ModuleResourcesManager::~ModuleResourcesManager()
{

}

bool ModuleResourcesManager::Init(const JSON_Object * config)
{
	update_ratio = 1000;
	return true;
}

bool ModuleResourcesManager::Start()
{
	GeneratePrimitiveResources();
	GenerateLibraryAndMeta();
	LoadFileToScene("Assets/Meshes/Street environment_V01.FBX");
	update_timer.Start();
	return true;
}

update_status ModuleResourcesManager::Update(float dt)
{
	ManageUITextures();
	if(update_timer.Read() > update_ratio){
		ManageAssetModification();
		update_timer.Start();
	}


	return UPDATE_CONTINUE;
}

update_status ModuleResourcesManager::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	// Debug purpuses
	if (cleanResources) {
		CleanMeta();
		CleanLibrary();
		cleanResources = false;
		App->CLOSE_APP();
	}

	return ret;
}




Resource * ModuleResourcesManager::newResource(resource_deff deff) {
	Resource* ret = nullptr;

	switch (deff.type) {
	case R_TEXTURE: ret = (Resource*) new ResourceTexture(deff); break;
	case R_MESH: ret = (Resource*) new ResourceMesh(deff); break;     
	case R_SCENE: ret = (Resource*) new ResourceScene(deff); break; 
	} 

	if (ret)
		resources[deff.uuid] = ret;

	return ret;
}

void ModuleResourcesManager::ManageUITextures() {
	for (auto it = resources.begin(); it != resources.end(); it++) {
		if ((*it).second->type == R_TEXTURE) {
			ResourceTexture* res_tex = (ResourceTexture*)(*it).second;
			if (res_tex->IsLoaded() && res_tex->components_used_by == 0) {
				if (res_tex->drawn_in_UI)	res_tex->drawn_in_UI = false;
				else						res_tex->UnloadFromMemory();
			}

			res_tex->drawn_in_UI = 0;
		}
	}
}

void ModuleResourcesManager::GeneratePrimitiveResources() {
	ResourceMesh* cube = new ResourceMesh(resource_deff());
	cube->mesh = new Mesh(PrimitiveTypes::Primitive_Cube);
	primitive_resources[Primitive_Cube] = cube;

	ResourceMesh* plane = new ResourceMesh(resource_deff());
	plane->mesh = new Mesh(PrimitiveTypes::Primitive_Plane);
	primitive_resources[Primitive_Plane] = plane;


	ResourceMesh* sphere = new ResourceMesh(resource_deff());
	sphere->mesh = new Mesh(PrimitiveTypes::Primitive_Sphere);
	primitive_resources[Primitive_Sphere] = sphere;

	ResourceMesh* cylinder = new ResourceMesh(resource_deff());
	cylinder->mesh = new Mesh(PrimitiveTypes::Primitive_Cylinder);
	primitive_resources[Primitive_Cylinder] = cylinder;
}


void ModuleResourcesManager::GenerateLibraryAndMeta()
{
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;

		resource_deff new_res_deff;
		if (ManageFile(it.path().generic_string(), new_res_deff))
			newResource(new_res_deff);
	}
}

bool ModuleResourcesManager::ManageFile(std::string file_path, resource_deff& deff)
{
	std::string path, name, extension;
	path = name = extension = file_path;	// Separate path, name and extension	
	App->fs.getExtension(extension);
	App->fs.getPath(path);
	App->fs.getFileNameFromPath(name);

	if (extension == JSON_EXTENSION) { // Scenes of our own engine are NOT to be exported to binary, they directly hold pointers to it.
		deff.requested_update = R_NOTHING;
		return false;
	}
	// Manage meta
	if (extension == META_EXTENSION) { // If it is a meta file
		deff = ManageMeta(path, name, extension);  // If has a corresponding asset, continue, else delete file from library and delete .meta
		return false;
	}
	// Manage asset
	deff = ManageAsset(path, name, extension);
	return true;
}

resource_deff ModuleResourcesManager::ManageMeta(std::string path, std::string name, std::string extension) {

	std::string full_meta_path = path + name + extension;
	resource_deff ret;

	std::string full_asset_path = path + name;

	if (App->fs.ExistisFile(full_asset_path.c_str())) // If file exists, MISSION ACOMPLISHED, return
	{
		ret.requested_update = R_NOTHING;
	}
	else {	
		JSON_Value* meta = json_parse_file(full_meta_path.c_str());
		std::string full_binary_path = json_object_get_string(json_object(meta), "binary_path");		// Delete file from library using the meta reference that points to it
		ResourceType r_type = type2enumType(json_object_get_string(json_object(meta), "type"));
		ret.uuid = json_object_get_number(json_object(meta), "resource_uuid");
		ret.requested_update = R_DELETE;
		if (r_type == R_SCENE) {									// Handle meshes delete when a scene is deleted
			CleanMeshesFromLibrary(full_binary_path.c_str());
		}
		App->fs.DestroyFile(full_binary_path.c_str());	// Destroy binary			
		App->fs.DestroyFile(full_meta_path.c_str()); // Destroy meta
		json_value_free(meta);
	}

	return ret;
}

void ModuleResourcesManager::CleanMeshesFromLibrary(std::string prefab_binary)
{
	JSON_Value* scene = json_parse_file(prefab_binary.c_str());
	JSON_Array* objects = json_object_get_array(json_object(scene), "Game Objects");

	// Delete meshes from library
	for (int i = 0; i < json_array_get_count(objects); i++) {
		JSON_Object* obj_deff = json_array_get_object(objects, i);
		JSON_Array* components = json_object_get_array(obj_deff, "Components");
		std::string mesh_binary;
		// Iterate components and look for the mesh
		bool mesh_found = false;
		for (int a = 0; a < json_array_get_count(components); a++) {
			JSON_Object* mesh_resource = json_array_get_object(components, a);
			std::string type = json_object_get_string(mesh_resource, "type");
			if (type == "mesh") {
				mesh_binary = json_object_get_string(mesh_resource, "mesh_binary_path");
				mesh_found = true;
				break;
			}
		}

		if (mesh_found) {					  // If a mesh was found destroy its binary
			App->fs.DestroyFile(mesh_binary.c_str());
		}
	}
	
}

resource_deff ModuleResourcesManager::ManageAsset(std::string path, std::string name, std::string extension) {

	JSON_Value* meta;
	int file_last_mod = 0;
	// Needed for collisioning .meta against asset
	std::string full_meta_path = path + name + extension + META_EXTENSION; // TODO: Add original extension to .meta
	std::string full_asset_path = path + name + extension;
	// Needed for import
	ResourceType enum_type;
	std::string uuid_str;
	std::string binary_path;
	uint uuid_number = 0;
	resource_deff deff;
	if (App->fs.ExistisFile(full_meta_path.c_str())) {		// Check if .meta file exists
		// EXISTING RESOURCE WITH POSSIBLE MODIFICATION
		meta = json_parse_file(full_meta_path.c_str());
		file_last_mod = App->fs.getFileLastTimeMod(full_asset_path.c_str());
		binary_path = json_object_get_string(json_object(meta), "binary_path");
		enum_type = type2enumType(json_object_get_string(json_object(meta), "type"));
		uuid_number = json_object_get_number(json_object(meta), "resource_uuid");
		uuid_str = uuid2string(uuid_number);
		deff.requested_update = R_UPDATE;
		if (json_object_get_number(json_object(meta), "timeCreated") == file_last_mod) { // Check if the last time that was edited is the .meta timestamp
			// EXISTING RESOURCE WITH NO MODIFICATION
			deff.set(uuid_number, enum_type, binary_path, full_asset_path);
			deff.requested_update = R_NOTHING;
			json_value_free(meta);
			return deff;																		// Existing meta, and timestamp is the same, generate a resource to store it in the code
		}
	}
	else {
		// NEW RESOURCE IN FOLDER
		App->fs.CreateEmptyFile(full_meta_path.c_str());	// If .meta doesn't exist, generate it
		meta = json_value_init_object();
		std::string str_type;
		uuid_number = random32bits();
		str_type = assetExtension2type(extension.c_str());
		enum_type = type2enumType(str_type.c_str());
		uuid_str = uuid2string(uuid_number);
		binary_path = App->fs.getPathFromLibDir(enumType2libDir(enum_type)) + uuid_str + enumType2binaryExtension(enum_type);
		file_last_mod = App->fs.getFileLastTimeMod(full_asset_path.c_str());
		json_object_set_number(json_object(meta), "resource_uuid", uuid_number); // Brand new uuid
		json_object_set_string(json_object(meta), "type", str_type.c_str()); // Brand new time
		json_object_set_string(json_object(meta), "binary_path", binary_path.c_str()); // Brand new binary path
		deff.requested_update = R_CREATE;
	}

	// If meta didn't exist, or existed but the asset was changed, 
	json_object_set_number(json_object(meta), "timeCreated", file_last_mod);	// Brand new / Updated time modification
	json_serialize_to_file(meta, full_meta_path.c_str());
	json_value_free(meta);

	switch (enum_type) {
		case R_TEXTURE:
			App->importer->ImportTexture(full_asset_path.c_str(), uuid_str);
			break;
		case R_SCENE:
			App->importer->ImportScene(full_asset_path.c_str(), uuid_str);
			break;
	}
	// Meta generated and file imported, create resource in code

	deff.set(uuid_number, enum_type, binary_path, full_asset_path);

	// Resource generated, MISSION ACOMPLISHED, return
	return deff;
}


void ModuleResourcesManager::LoadResource(uint uuid) {
	auto it = resources.find(uuid);
	if (it != resources.end()) {
		resources[uuid]->LoadToMemory();
	}
	else
		app_log->AddLog("WARNING: Trying to load non existing resource");
}

void ModuleResourcesManager::ManageAssetModification()
{
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;
		
		resource_deff deff;
		if (ManageFile(it.path().generic_string(), deff)) {
			switch (deff.requested_update) {
				case R_CREATE:
					newResource(deff);
					break;
				case R_UPDATE:
					resources[deff.uuid]->UnloadFromMemory();
					resources[deff.uuid]->LoadToMemory();
					break;
			}
		}
		else {
			if(deff.requested_update == R_DELETE){
				auto it = resources.find(deff.uuid);
				if (it != resources.end()) {
					Resource* delete_res = resources[deff.uuid];
					delete_res->UnloadFromMemory();
					delete delete_res;
					resources.erase(it);
				}
			}
		}
	}

}

Resource * ModuleResourcesManager::getResource(uint uuid) {

	Resource* ret = nullptr;
	auto it = resources.find(uuid);
	if (it != resources.end()) {
		ret = resources[uuid];
	}
	else
		app_log->AddLog("WARNING: Asking for non existing resource");

	return ret;
}

Resource * ModuleResourcesManager::getPrimitiveMeshResource(PrimitiveTypes type) {
	return primitive_resources[type];
}

int ModuleResourcesManager::assignResource(uint uuid) {

	auto it = resources.find(uuid);
	if (it != resources.end()) {
		resources[uuid]->components_used_by++;
		if (!resources[uuid]->IsLoaded()) {
			resources[uuid]->LoadToMemory();
		}
	}
	else
		app_log->AddLog("WARNING: Trying to assing non existing resource");

	return 0;
}

int ModuleResourcesManager::deasignResource(uint uuid) {
	auto it = resources.find(uuid);
	if (it != resources.end()) {
		resources[uuid]->components_used_by--;
		if (resources[uuid]->components_used_by == 0) {
			resources[uuid]->UnloadFromMemory();
		}
	}
	else
		app_log->AddLog("WARNING: Trying to deasign non existing resource");

	return 0;
}

void ModuleResourcesManager::LoadFileToScene(const char * file) {
	std::string full_meta_path = file;
	full_meta_path += META_EXTENSION;
	uint resource_uuid = -1;

	if (App->fs.ExistisFile(full_meta_path.c_str())) {
		JSON_Value* meta = json_parse_file(full_meta_path.c_str());
		resource_uuid = json_object_get_number(json_object(meta), "resource_uuid");
		json_value_free(meta);

		resources[resource_uuid]->LoadToMemory();
	}
	else{
		app_log->AddLog("%s has no .meta, can't be loaded", file);
		return;
	}


	
}

uint ModuleResourcesManager::getResourceUuid(const char * file) {
	std::string full_meta_path = file;
	full_meta_path += META_EXTENSION;
	uint ret = 0;
	if (App->fs.ExistisFile(full_meta_path.c_str())) {
		JSON_Value* meta = json_parse_file(full_meta_path.c_str());
		ret = json_object_get_number(json_object(meta), "resource_uuid");
		json_value_free(meta);
	}
	else {
		app_log->AddLog("%s has no .meta, can't be loaded", file);
	}

	return ret;
}

void ModuleResourcesManager::CleanMeta() {
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;

		std::string path, name, extension;
		path = name = extension = it.path().generic_string();	// Separate path, name and extension	
		App->fs.getExtension(extension);
		App->fs.getPath(path);
		App->fs.getFileNameFromPath(name);

		if (extension == META_EXTENSION)
			App->fs.DestroyFile((path + name + extension).c_str());

	}
}

void ModuleResourcesManager::CleanLibrary()
{
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(LIBRARY_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;
		App->fs.DestroyFile(it.path().generic_string().c_str());
	}
}

void ModuleResourcesManager::getMeshResourceList(std::list<resource_deff>& meshes) {
	for (auto it = resources.begin(); it != resources.end(); it++) {
		if ((*it).second->type == R_MESH) {
			Resource* curr = (*it).second;
			resource_deff deff(curr->uuid, curr->type, curr->binary, curr->asset);
			meshes.push_back(deff);
		}
	}
}

std::string ModuleResourcesManager::uuid2string(uint uuid) {
	return std::to_string(uuid);
}

const char * ModuleResourcesManager::assetExtension2type(const char * _extension) {

	char* ret = "unknown";

	std::string extension = _extension;

	if (extension == ".FBX" || extension == ".fbx" || extension == ".dae" || extension == ".blend" || extension == ".3ds" || extension == ".obj"
		|| extension == ".gltf" || extension == ".glb" || extension == ".dxf" || extension == ".x") 
		ret = "scene";
	else if (extension == ".bmp" || extension == ".dds" || extension == ".jpg" || extension == ".pcx" || extension == ".png"
		|| extension == ".raw" || extension == ".tga" || extension == ".tiff") 
		ret = "texture";
	else if (extension == ".json")
		ret = "json";
	else if (extension == ".meta")
		ret = "meta";

	return ret;
}

ResourceType ModuleResourcesManager::type2enumType(const char * type) {
	ResourceType ret = R_UNKNOWN;
	std::string str_type = type;

	if (str_type == "scene")
		ret = R_SCENE;
	if (str_type == "texture")
		ret = R_TEXTURE;

	return ret;
}

const char * ModuleResourcesManager::enumType2binaryExtension(ResourceType type) {
	const char* ret = "";
	switch (type) {
		case R_TEXTURE:
			ret = ".dds";
			break;
		case R_MESH:
			ret = ".kr";
			break;
		case R_SCENE:
			ret = ".json";
			break;
	}

	return ret;
}

lib_dir ModuleResourcesManager::enumType2libDir(ResourceType type) {
	lib_dir ret = NO_LIB;
	switch (type) {
	case R_TEXTURE:
		ret = LIBRARY_TEXTURES;
		break;
	case R_MESH:
		ret = LIBRARY_MESHES;
		break;
	case R_SCENE:
		ret = LIBRARY_PREFABS;
		break;
	}
	return ret;
}


bool ModuleResourcesManager::CleanUp() {
	// Unload all memory from resources and delete resources
	for (auto it = resources.begin(); it != resources.end(); it++) {
		if (Resource* resource = (*it).second) {
			if (resource->IsLoaded())
				resource->UnloadFromMemory();
			delete resource;
		}
	}
	resources.clear();

	for (auto it = primitive_resources.begin(); it != primitive_resources.end(); it++) {
		Resource* resource = (*it).second;
		resource->UnloadFromMemory();
		delete resource;
	}

	return true;
}
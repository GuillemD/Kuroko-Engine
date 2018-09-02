#pragma once
#include "Module.h"
#include "Globals.h"
#include "imgui.h"

struct ImGuiIO;
class GameObject;

class ModuleImGUI :
	public Module {
public:
	ModuleImGUI(Application* app, bool start_enabled = true);
	~ModuleImGUI();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void DrawGraphicsTab();
	void DrawHierarchyTab();
	void DrawHierarchyNode(GameObject* game_object, int& id);

private:
	bool show_demo_window	= true;
	bool show_graphic_tab	= true;
	bool show_test_tab		= true;
	bool show_hierarchy_tab = true;
	ImGuiIO io;
};


#ifndef _COMPONENTAABB_
#define _COMPONENTAABB_

#include "Vector3.h"
#include "Component.h"
#include "Color.h"

#include <list>

class ComponentTransform;

class ComponentAABB : public Component
{
public:
	ComponentAABB(GameObject* parent) : Component(parent, C_AABB) { Reload(); };
	~ComponentAABB();

	bool Update(float dt);
	void Reload();

	math::AABB* getAABB() const { return aabb; };
	math::OBB* getOBB() const { return obb; };

	void DrawAABB();
	void DrawOBB();


public:
	bool draw_aabb = false;
	bool draw_obb = false;


private: 

	math::AABB* aabb = nullptr;
	math::OBB* obb = nullptr;

	ComponentTransform* transform = nullptr;

};
#endif
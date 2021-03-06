#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Component.h"
#include "MathGeoLib\Math\float3.h"

class Camera;
class ComponentTransform;

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent, Camera* camera);
	~ComponentCamera();

	bool Update(float dt);
	Camera* getCamera() const { return camera; };

public:

	float3 offset = float3::zero;

private:

	Camera* camera = nullptr;
	ComponentTransform* transform = nullptr;

};

#endif
#include "Camera.h"
#include "GameObject.h"
#include "MathGeoLib\Math\Quat.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ModuleScene.h"

Camera::Camera(float3 position, float3 reference, float n_plane, float f_plane, float hor_fov, float ver_fov)
{

	frustum = new Frustum();
	frustum->pos = position;
	frustum->front = float3::unitZ;
	frustum->up = float3::unitY;
	frustum->nearPlaneDistance = n_plane;
	frustum->farPlaneDistance = f_plane;
	frustum->type = PerspectiveFrustum;
	frustum->orthographicHeight = ver_fov;				frustum->orthographicWidth = hor_fov;
	frustum->verticalFov = DEGTORAD * ver_fov;			frustum->horizontalFov = DEGTORAD * hor_fov;

	LookAt(reference);
	updateFrustum();
}


// -----------------------------------------------------------------
void Camera::LookAt(const float3 &Spot)
{
	Reference = Spot;

	Z = (frustum->pos - Reference).Normalized();
	X = float3::unitY.Cross(Z).Normalized();
	Y = Z.Cross(X);

	updateFrustum();

}


void Camera::Move(const float3 &Movement)
{
	frustum->pos += Movement;
	Reference += Movement;

	updateFrustum();
}

void Camera::LookAtSelectedGeometry()
{
	if (GameObject* selected_obj = App->scene->selected_obj)
	{
		float3 centroid = float3::zero;
		selected_obj->getInheritedHalfsizeAndCentroid(float3(), centroid);
		LookAt(centroid);
	}

	updateFrustum();
}

void Camera::FitToSizeSelectedGeometry(float distance)
{
	if (GameObject* selected_obj = App->scene->selected_obj)
	{
		float3 centroid = float3::zero; float3 half_size = float3::zero;
		selected_obj->getInheritedHalfsizeAndCentroid(half_size, centroid);

		float3 new_pos = centroid + half_size + float3(distance, distance, distance);
		new_pos = Quat::RotateY(((ComponentTransform*)selected_obj->getComponent(TRANSFORM))->getRotationEuler().y) * new_pos;

		frustum->pos = new_pos;
		LookAt(centroid);
	}
	else
		LookAt(float3::zero);

	updateFrustum();
}

void Camera::Reset()
{
	frustum->pos = float3(1.0f, 1.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);

	Z = (frustum->pos - Reference).Normalized();
	X = float3::unitY.Cross(Z).Normalized();
	Y = Z.Cross(X);

	LookAt(Reference);

	updateFrustum();
}


void Camera::updateFrustum()
{
	frustum->front = -Z;
	frustum->up = Y;
}

void  Camera::setFov(float hor_fov, float ver_fov)
{
	frustum->horizontalFov = hor_fov; frustum->verticalFov = ver_fov;
}

void  Camera::setPlaneDistance(float n_plane, float f_plane)
{
	frustum->nearPlaneDistance = n_plane; frustum->farPlaneDistance = f_plane;
}
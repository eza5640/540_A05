#include "Camera.h"
Camera::Camera()
{
	F = XMVectorSet(0.0f, 0.0f, 1.f, 0.0f);
	xRot = 0.f;
	yRot = 0.f;
}
void Camera::Update(float deltaTime)
{
	XMVECTOR UP = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR L = XMQuaternionRotationRollPitchYaw(xRot, yRot, 0.0f);
	XMVECTOR P = L * F;
	if (xRot != 0.f && yRot != 0.f)
	{
		direction = P;		
	}
	else
	{
		direction = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	}
	
	UP = XMVector3Cross(XMVector3Cross(F, UP), F);
	XMMATRIX V = XMMatrixLookToLH(
		position,     // The position of the "camera"
		direction,     // Direction the camera is looking
		UP);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&view, XMMatrixTranspose(V)); // Transpose for HLSL!
	F = XMVectorSet(sin(yRot) * cos(xRot), sin(-xRot), cos(xRot) * cos(yRot), 0.f);
	if (GetAsyncKeyState('W') & 0x8000) 
	{
		XMVECTOR Z = XMVectorSet(0.f, 0.f, 1.0f * deltaTime, 0.f);
		XMFLOAT3 ZDir;
		XMStoreFloat3(&ZDir, direction);
		if (ZDir.z == 0.f)
		{
			
			//XMFLOAT3 z = XMFLOAT3(0.f, 0.f, 0.2f);
			//XMFLOAT3 dir;
			//XMStoreFloat3(&dir, direction);
			//dir *= z;
			position += Z;
		}
		else
		{
			position += Z * direction;
		}
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		XMVECTOR X = XMVectorSet(-1.0f * deltaTime, 0.f, 0.f, 0.f);
		XMFLOAT3 XDir;
		XMStoreFloat3(&XDir, direction);
		if (XDir.x == 0.f)
		{

			//XMFLOAT3 z = XMFLOAT3(0.f, 0.f, 0.2f);
			//XMFLOAT3 dir;
			//XMStoreFloat3(&dir, direction);
			//dir *= z;
			position += X;
		}
		else
		{
			position += X * direction;
		}
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		XMVECTOR Z = XMVectorSet(0.f, 0.f, -1.0f * deltaTime, 0.f);
		XMFLOAT3 ZDir;
		XMStoreFloat3(&ZDir, direction);
		if (ZDir.z == 0.f)
		{

			//XMFLOAT3 z = XMFLOAT3(0.f, 0.f, 0.2f);
			//XMFLOAT3 dir;
			//XMStoreFloat3(&dir, direction);
			//dir *= z;
			position += Z;
		}
		else
		{
			position += Z * direction;
		}
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		XMVECTOR X = XMVectorSet(1.0f * deltaTime, 0.f, 0.f, 0.f);
		XMFLOAT3 XDir;
		XMStoreFloat3(&XDir, direction);
		if (XDir.x == 0.f)
		{

			//XMFLOAT3 z = XMFLOAT3(0.f, 0.f, 0.2f);
			//XMFLOAT3 dir;
			//XMStoreFloat3(&dir, direction);
			//dir *= z;
			position += X;
		}
		else
		{
			position += X * direction;
		}
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, position);
		pos.y -= 1.0f * deltaTime;
		position = XMLoadFloat3(&pos);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, position);
		pos.y += 1.0f * deltaTime;
		position = XMLoadFloat3(&pos);
	}
}
XMFLOAT4X4 Camera::GetView()
{
	return view;
}

void Camera::SetView(XMMATRIX v)
{
	XMStoreFloat4x4(&view, XMMatrixTranspose(v));
}

void Camera::RotateCam(int xRotate, int yRotate)
{
	xRot += (float)(yRotate) / 25.f;
	yRot += (float)(xRotate) / 25.f;
	if (xRot > 30.f)
	{
		xRot = 30.f;
	}
	else if (xRot < -30.f)
	{
		xRot = -30.f;
	}
	if (yRot > 30.f)
	{
		yRot = 30.f;
	}
	else if (yRot < -30.f)
	{
		yRot = -30.f;
	}
}

void Camera::SetProj(float width, float height)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P)); // Transpose for HLSL!
}
XMFLOAT4X4 Camera::GetProj()
{
	return projection;
}

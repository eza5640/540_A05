#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include "DXCore.h"
#include <iostream>
using namespace DirectX;
class Camera
{
public:
	Camera();
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT4X4 GetView();
	void SetView(XMMATRIX);
	void SetProj(float, float);
	XMFLOAT4X4 GetProj();
	XMVECTOR position = XMVectorSet(0, 0, -25, 0);
	XMVECTOR direction = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR F;
	float xRot;
	float yRot;
	void Update(float);
	void RotateCam(int, int);
};


#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include "Mesh.h"
#include "DXCore.h"
#include "Material.h"
using namespace DirectX;
class GameEntity
{	
public:
	XMMATRIX worldTemp;
	XMFLOAT3 pos;
	XMFLOAT3 scale;
	XMFLOAT3 rot;
	XMFLOAT4X4 world;
	Mesh* gameMesh;
	Material* material;
	GameEntity(Mesh* mesh, Material* mat);
	~GameEntity();
	XMFLOAT4X4 GetWorld();
	XMFLOAT3 GetRot();
	XMFLOAT3 GetPos();
	XMFLOAT3 GetScale();
	void SetRot(XMFLOAT3);
	void SetPos(XMFLOAT3);
	void SetScale(XMFLOAT3);
	void SetWorld(XMMATRIX);
	void Move(XMFLOAT3);
	void Rotate(XMFLOAT3);
	void Scale(XMFLOAT3);
	void PrepareMaterial(XMFLOAT4X4, XMFLOAT4X4);
};



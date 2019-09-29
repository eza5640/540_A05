#include "GameEntity.h"
GameEntity::GameEntity(Mesh* mesh, Material* mat)
{
	gameMesh = mesh;
	material = mat;
	worldTemp = XMMatrixIdentity();
	XMStoreFloat4x4(&world, XMMatrixTranspose(worldTemp));
	scale = XMFLOAT3(1.f, 1.f, 1.f);
	pos = XMFLOAT3(0.f, 0.f, 0.f);
	rot = XMFLOAT3(0.f, 0.f, 0.f);
}
GameEntity::~GameEntity()
{
	
}

XMFLOAT4X4 GameEntity::GetWorld()
{
	return world;
}

XMFLOAT3 GameEntity::GetPos()
{
	return pos;
}

XMFLOAT3 GameEntity::GetRot()
{
	return rot;
}

XMFLOAT3 GameEntity::GetScale()
{
	return scale;
}

void GameEntity::SetPos(XMFLOAT3 newPos)
{
	pos = newPos;
	XMMATRIX newMat = XMMatrixIdentity() * XMMatrixTranslation(newPos.x, newPos.y, newPos.z);
	SetWorld(newMat);
}

void GameEntity::SetRot(XMFLOAT3 newRot)
{
	rot = newRot;
	XMMATRIX newMat = XMMatrixIdentity() * XMMatrixRotationRollPitchYaw(newRot.x, newRot.y, newRot.z);
	SetWorld(newMat);
}

void GameEntity::SetScale(XMFLOAT3 newScale)
{
	scale = newScale;
	XMMATRIX newMat = XMMatrixIdentity() * XMMatrixScaling(newScale.x, newScale.y, newScale.z);
	SetWorld(newMat);
}

void GameEntity::SetWorld(XMMATRIX newWorld)
{
	XMStoreFloat4x4(&world, XMMatrixTranspose(newWorld));
}

void GameEntity::Move(XMFLOAT3 move)
{
	SetPos(XMFLOAT3(pos.x + move.x, pos.y + move.y, pos.z + move.z));
}

void GameEntity::Rotate(XMFLOAT3 rotate)
{
	SetRot(XMFLOAT3(rot.x + rotate.x, rot.y + rotate.y, rot.z + rotate.z));
}

void GameEntity::Scale(XMFLOAT3 scaled)
{
	SetScale(XMFLOAT3(scale.x + scaled.x, scale.y + scaled.y, scale.z + scaled.z));
}

void GameEntity::PrepareMaterial(XMFLOAT4X4 view, XMFLOAT4X4 proj)
{
	material->GetVShader()->SetMatrix4x4("world", this->GetWorld());
	material->GetVShader()->SetMatrix4x4("view", view);
	material->GetVShader()->SetMatrix4x4("projection", proj);

	material->GetVShader()->CopyAllBufferData();
	material->GetVShader()->SetShader();
	material->GetPShader()->SetShader();
}

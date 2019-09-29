#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include <iostream>
#include <fstream>
#include <vector>

class Mesh
{
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	int numIndices;
public:
	Mesh(Vertex* vertices, int numVertices, unsigned int* indices, int numIndex, ID3D11Device* device);
	Mesh(const char*, ID3D11Device* device);
	~Mesh();
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
};


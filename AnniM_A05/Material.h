#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include "DXCore.h"
#include "SimpleShader.h"
class Material
{
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
public:
	Material(SimpleVertexShader*, SimplePixelShader*);
	SimplePixelShader* GetPShader();
	SimpleVertexShader* GetVShader();
};


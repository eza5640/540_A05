#include "Material.h"
Material::Material(SimpleVertexShader* vShade, SimplePixelShader* pShade)
{
	vertexShader = vShade;
	pixelShader = pShade;
}
SimplePixelShader* Material::GetPShader()
{
	return pixelShader;
}

SimpleVertexShader* Material::GetVShader()
{
	return vertexShader;
}
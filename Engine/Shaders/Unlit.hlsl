#include "Constants.hlsl"

struct VIn
{
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float2 tex : VPOS;
};

struct VOut
{
	float4 worldPosition : POSITION1;
	float4 position : SV_POSITION;
	float2 tex : VPOS;
	float3 normal : NORMAL0;
};

VOut VS(VIn vIn)
{
	VOut output;

	output.position = mul(float4(vIn.position, 1.0), c_modelToWorld);

	output.worldPosition = output.position;
	float4 normalTemp = mul(float4(vIn.normal, 0.0), c_modelToWorld);
	output.normal = float3(normalTemp.x, normalTemp.y, normalTemp.z);

	output.position = mul(output.position, c_viewProj);

	output.tex = vIn.tex;

	return output;
}

float4 PS(VOut pIn) : SV_TARGET
{

	clamp(pIn.tex.x, 0, 1);
	clamp(pIn.tex.y, 0, 1);

	float4 texColor = DiffuseTexture.Sample(DefaultSampler, pIn.tex);


	return texColor;
}

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

	float4 returnColor;
	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if (c_pointLight[i].isEnabled)
		{
			float3 L = c_pointLight[i].position - float3(pIn.worldPosition.x, pIn.worldPosition.y, pIn.worldPosition.z);
			float lengt = length(L);
			if (lengt >= c_pointLight[i].outerRadius)
			{
				lengt = 0;
			}

			else if (lengt <= c_pointLight[i].innerRadius)
			{
				lengt = 1;
			}

			else
			{
				lengt = smoothstep(c_pointLight[i].outerRadius, c_pointLight[i].innerRadius, lengt);
			}
			L = normalize(L);

			float3 V = c_cameraPosition - float3(pIn.worldPosition.x, pIn.worldPosition.y, pIn.worldPosition.z);
			V = normalize(V);

			float3 norm = normalize(pIn.normal);
			float3 R = reflect(-L, norm);
			R = normalize(R);

			float dotRes = mul(norm, L);
			dotRes = max(dotRes, 0.0);
			float3 diffuse = c_pointLight[i].diffuseColor * dotRes;
			diffuse = diffuse * lengt;
			float3 dotRes2 = mul(R, V);
			dotRes2 = max(dotRes2, 0.0);
			float3 PowRes = pow(dotRes2, c_pointLight[i].specularPower);
			float3 specular = c_pointLight[i].specularColor * PowRes;
			specular = specular * lengt;
			float3 LightResult = c_ambient + diffuse + specular;
			returnColor = returnColor + (texColor * float4(LightResult, 1.0));
		}
	}

	return returnColor;
}

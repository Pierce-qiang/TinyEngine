#version 430 core

struct Material 
{
	sampler2D texture_albedo;
	sampler2D texture_normal;
	sampler2D texture_metallic;
	sampler2D texture_roughness;
	sampler2D texture_ao;
};

// input and output
in vec3 fragPos;
in mat3 TBN;
in vec3 baseNormal; // for the case with out normal mapping
in vec2 texCoords;

layout (location = 0) out vec4 gb_Albedo;
layout (location = 1) out vec3 gb_Normal;
layout (location = 2) out vec3 gb_Pos;
layout (location = 3) out vec3 gb_Mixture;

uniform Material material;
uniform int useNormalMapping;

void main()
{
	vec4 albedo = texture(material.texture_albedo, texCoords);
	vec3 normal = texture(material.texture_normal, texCoords).rgb;
	normal = normal * 2.0f - 1.0f;
	normal = normalize(TBN * normal);
	if(useNormalMapping == 0)
		normal = baseNormal; 

	float metallic = texture(material.texture_metallic, texCoords).r;
	float roughness = texture(material.texture_roughness, texCoords).r;
	float ao = texture(material.texture_ao, texCoords).r;

	gb_Albedo = albedo;
	gb_Normal = normal;
	gb_Pos = fragPos;
	gb_Mixture = vec3(roughness, metallic, ao);
}

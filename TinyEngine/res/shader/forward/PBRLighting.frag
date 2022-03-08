#version 430 core

// struct define
struct Material 
{
	sampler2D texture_albedo;
	sampler2D texture_normal;
	sampler2D texture_metallic;
	sampler2D texture_roughness;
	sampler2D texture_ao;
};

struct DirLight 
{
	vec3 direction;
	float intensity;
	vec3 lightColor;
};

struct PointLight 
{
	vec3 position;
	float intensity;
	vec3 lightColor;
	float attenuationRadius;
};

struct SpotLight 
{
	vec3 position;
	vec3 direction;
	float intensity;
	vec3 lightColor;
	float attenuationRadius;

	float innerCutOff;
	float outerCutOff;
};

#define MAX_DIR_LIGHTS 5
#define MAX_POINT_LIGHTS 5
#define MAX_SPOT_LIGHTS 5
const float PI = 3.14159265359;

// input and output
in mat3 TBN;
in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;
in vec4 fragPosLightSpace;
out vec4 FragColor;

// lighting
uniform sampler2D shadowMap;
uniform ivec3 numDirPointSpotLights;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 viewPos;
uniform Material material;

// Light radiance calculations
vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);
vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);
vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);

// Cook-Torrance BRDF functions adopted by Epic for UE4
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness) ;
float GeometrySchlickGGX(float cosTheta, float roughness);
float GeometrySmith(float roughness, float NdotL, float NdotV);
vec3 FresnelSchlick(float HdotV, vec3 F0) ;

// Shadow factor cal
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

// Tonemapping
vec3 ACESTonemapping(vec3 value);

void main()
{
	// sample textures to get basic parameters
	vec3 albedo = texture(material.texture_albedo, texCoords).rgb;
	vec3 normal = texture(material.texture_normal, texCoords).rgb;
	normal = normal * 2.0f - 1.0f;
	normal = normalize(TBN * normal);
	float roughness = texture(material.texture_roughness, texCoords).r;
	float metallic = texture(material.texture_metallic, texCoords).r;
	float ao = texture(material.texture_ao, texCoords).r;

	// view direction and F0
	vec3 fragToView = normalize(viewPos - fragPos); // view direction
	vec3 F0 = mix(vec3(0.04f), albedo, metallic);

	// calculate direct radiance
	vec3 directLightRadiance = vec3(0.0);

	// only support one directional light and it's shadow now
	directLightRadiance += CalculateDirectionalLightRadiance(albedo,normal,metallic,roughness,fragToView,F0);
	vec3 color = ACESTonemapping(directLightRadiance);
	color = pow(color, vec3(1.0f/2.2f));
	FragColor = vec4(color, 1.0f);
}

vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 F0)
{
	vec3 directLightRadiance = vec3(0.0);

	// only one directional light and it's shadow now
	for (int i = 0; i < numDirPointSpotLights.x; i++) 
	{
		vec3 lightDir = normalize(dirLights[i].direction);
		vec3 halfway = normalize(lightDir + fragToView);
		vec3 radiance = dirLights[i].intensity * dirLights[i].lightColor;

		float NdotL = max(dot(normal, lightDir), 0.0f);
		float NdotV = max(dot(normal, fragToView), 0.0f);
		float HdotV = max(dot(halfway, fragToView), 0.0f);

		// specular
		vec3 F = FresnelSchlick(HdotV, F0);
		float D = NormalDistributionGGX(normal, halfway, roughness);
		float G = GeometrySmith(roughness, NdotL, NdotV);
		vec3 numerator = F * D * G;
		float denominator = max(4 * NdotL * NdotV, 0.0f) + 0.001f;
		vec3 specular = numerator / denominator;

		// diffuse
		vec3 kD = (1.0f - F0) * (1 - metallic);
		vec3 diffuse = kD * albedo / PI;

		// take care shadow
		float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir); 
		directLightRadiance += (diffuse + specular) * radiance * NdotL * (1 - shadow);
	}

	return directLightRadiance;
}


// Approximates the amount of microfacets that are properly aligned with the halfway vector, thus determines the strength and area for specular light
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness) 
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(normal, halfway), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = NdotH2 * (a2 - 1.0f) + 1.0f;
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}


// Approximates the geometry obstruction and geometry shadowing respectively, on the microfacet level
float GeometrySchlickGGX(float cosTheta, float roughness) 
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float numerator = cosTheta;
	float denominator = cosTheta * (1.0 - k) + k;

	return numerator / denominator;
}

float GeometrySmith(float roughness, float NdotL, float NdotV)
{
	return GeometrySchlickGGX(NdotL, roughness) * GeometrySchlickGGX(NdotV, roughness);
}

// Calculates the amount of specular light. Since diffuse(refraction) and specular(reflection) are mutually exclusive, 
// we can also use this to determine the amount of diffuse light
vec3 FresnelSchlick(float HdotV, vec3 F0) 
{
	float invcCosTheta = 1.0f - HdotV;
	return  F0 + (1.0f - F0) * (invcCosTheta * invcCosTheta * invcCosTheta * invcCosTheta * invcCosTheta);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
   shadow /= 9.0;
   if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

vec3 ACESTonemapping(vec3 value)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	value = (value * (a * value + b)) / (value * (c * value + d) + e);
	return clamp(value, 0.0f, 1.0f);
}
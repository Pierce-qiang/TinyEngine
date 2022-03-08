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

// input and output
in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;
in vec4 fragPosLightSpace;
out vec4 FragColor;

// lighting
uniform sampler2D shadowMap;
uniform vec3 directionalLightDir;
uniform vec3 viewPos;
uniform Material material;


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
	for(int x = -5; x <= 5; ++x)
	{
		for(int y = -5; y <= 5; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
   shadow /= 25.0;
   if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{
	vec3 color = texture(material.texture_albedo, texCoords).rgb;
	vec3 ambient, diffuse, specular;

	// ambient
	ambient = 0.05f * color; 

	// diffuse
	vec3 lightDir = normalize(directionalLightDir); // directional light
	float NdotL = max(dot(lightDir, normal), 0.0f);
	diffuse = color * NdotL;

	// specular;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 h = normalize(lightDir + viewDir);
	float NdotH = max(dot(normal,h), 0.0f);
	specular = vec3(0.3) * pow(NdotH, 64.0f);
	
    // calculate shadow
    float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);   
	FragColor = vec4(ambient + (1-shadow)*(diffuse + specular) , 1.0f);
	// FragColor = vec4(color, 1.0f);
}

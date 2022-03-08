#version 430 core

// struct define
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

// view position and shadowmap
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

#define MAX_DIR_LIGHTS 10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10
#define PI 3.141592653589793
#define PI2 6.283185307179586
vec2 textureResolution = textureSize(shadowMap, 0);

// input and output
in vec2 texCoords;
out vec4 FragColor;

// gbuffer
uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D mixtureTexture;
// uniform sampler2D ssaoTexture;


// lighting
uniform ivec3 numDirPointSpotLights;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// Light radiance calculations
vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, 
float roughness, vec3 fragToView, vec3 F0, vec4 fragPosLightSpace);
vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToView, vec3 F0);
vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToView, vec3 F0);

// Cook-Torrance BRDF functions adopted by Epic for UE4
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness) ;
float GeometrySchlickGGX(float cosTheta, float roughness);
float GeometrySmith(float roughness, float NdotL, float NdotV);
vec3 FresnelSchlick(float HdotV, vec3 F0) ;

// Shadow factor cal
float ShadowCalculation(vec3 coords);

//---------------pcss---------------
//poisson sample and pcss
#define NUM_SAMPLES 25
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10.0
#define LIGHT_WIDTH 25.0
vec2 poissonDisk[NUM_SAMPLES];
float PCF(vec3 coords, float filtersize);
float PCSS(vec3 coords);
//----------------------------------------

// Tonemapping
vec3 ACESTonemapping(vec3 value);

void main()
{
	// sample textures to get basic parameters
	vec3 albedo = texture(albedoTexture, texCoords).rgb;
	vec3 normal = normalize(texture(normalTexture, texCoords).rgb);
	float roughness = texture(mixtureTexture, texCoords).r;
	float metallic = texture(mixtureTexture, texCoords).g;
	float ao = texture(mixtureTexture, texCoords).b;

	// view direction and F0
	vec3 fragPos = texture(positionTexture, texCoords).rgb; 
	vec3 fragToView = normalize(viewPos - fragPos);
	vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos,1.0f);
	vec3 F0 = mix(vec3(0.04f), albedo, metallic);

	// calculate direct radiance
	vec3 directLightRadiance = vec3(0.0);
	vec3 directPointLightRadiance = vec3(0.0);

	// only support one directional light shadow now
	directLightRadiance += CalculateDirectionalLightRadiance(albedo,normal,metallic,roughness,fragToView,F0,fragPosLightSpace);
	directLightRadiance += CalculatePointLightRadiance(albedo,normal,metallic,roughness,fragPos, fragToView,F0);
	directLightRadiance += CalculateSpotLightRadiance(albedo,normal,metallic,roughness,fragPos, fragToView,F0);
	
	vec3 color = ACESTonemapping(directLightRadiance);
	color = pow(color, vec3(1.0f/2.2f));
	FragColor = vec4(color, 1.0f);
}

vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 F0, vec4 fragPosLightSpace)
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
		vec3 kD = (vec3(1.0f) - F) * (1 - metallic);
		vec3 diffuse = kD * albedo / PI;



		// perform perspective divide
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
		// transform to [0,1] range
		projCoords = projCoords * 0.5 + 0.5;
		// check whether current frag pos is in shadow
		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
		projCoords.z -= bias;

		if(projCoords.z > 1.0) {
			directLightRadiance += (diffuse + specular) * radiance * NdotL;
			continue;
		}

		//TODO: swich shadow cal mode here!-------------------

		// shadow just sample near 8 sample
		// float shadow = ShadowCalculation(projCoords); 
		// directLightRadiance += (diffuse + specular) * radiance * NdotL * (1 - shadow);

		//pcf
		// float visibility = PCF(projCoords, 2.0);
		// directLightRadiance += (diffuse + specular) * radiance * NdotL * visibility;

		// pcss
		float visibility = PCSS(projCoords);
		directLightRadiance += (diffuse + specular) * radiance * NdotL * visibility;
	}

	return directLightRadiance;
}

vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToView, vec3 F0)
{
	vec3 pointLightIrradiance = vec3(0.0);
	for (int i = 0; i < numDirPointSpotLights.y; ++i) 
	{
		vec3 fragToLight = normalize(pointLights[i].position - fragPos);
		vec3 halfway = normalize(fragToView + fragToLight);
		float fragToLightDistance = length(pointLights[i].position - fragPos);

		// Attenuation calculation (based on Epic's UE4 falloff model)
		float d = fragToLightDistance / pointLights[i].attenuationRadius;
		float d2 = d * d;
		float d4 = d2 * d2;
		float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);
		float attenuation = (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
		vec3 radiance = pointLights[i].intensity * pointLights[i].lightColor * attenuation;
		
		// Cook-Torrance Specular BRDF calculations
		// specular
		float NdotL = max(dot(normal, fragToLight), 0.0f);
		float NdotV = max(dot(normal, fragToView), 0.0f);
		float HdotV = max(dot(halfway, fragToView), 0.0f);

		vec3 F = FresnelSchlick(HdotV, F0);
		float D = NormalDistributionGGX(normal, halfway, roughness);
		float G = GeometrySmith(roughness, NdotL, NdotV);
		vec3 numerator = F * D * G;
		float denominator = max(4 * NdotL * NdotV, 0.0f) + 0.001f;
		vec3 specular = numerator / denominator;

		// diffuse
		vec3 kD = (vec3(1.0f) - F) * (1 - metallic);
		vec3 diffuse = kD * albedo / PI;

		// Add the light's radiance to the irradiance sum
		pointLightIrradiance += (diffuse + specular) * radiance * NdotL;
	}

	return pointLightIrradiance;
}


vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToView, vec3 F0)
{
	vec3 spotLightIrradiance = vec3(0.0);
	for (int i = 0; i < numDirPointSpotLights.z; ++i) 
	{
		vec3 fragToLight = normalize(spotLights[i].position - fragPos);
		vec3 halfway = normalize(fragToView + fragToLight);
		float fragToLightDistance = length(spotLights[i].position - fragPos);

		// Attenuation calculation (based on Epic's UE4 falloff model)
		float d = fragToLightDistance / spotLights[i].attenuationRadius;
		float d2 = d * d;
		float d4 = d2 * d2;
		float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);

		// Check if it is in the spotlight's circle
		float theta = dot(normalize(spotLights[i].direction), -fragToLight);
		float difference = spotLights[i].innerCutOff - spotLights[i].outerCutOff;
		float intensity = clamp((theta - spotLights[i].outerCutOff) / difference, 0.0, 1.0);
		float attenuation = intensity * (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
		vec3 radiance = spotLights[i].intensity * spotLights[i].lightColor * attenuation;

		// Cook-Torrance Specular BRDF calculations
		// specular
		float NdotL = max(dot(normal, fragToLight), 0.0f);
		float NdotV = max(dot(normal, fragToView), 0.0f);
		float HdotV = max(dot(halfway, fragToView), 0.0f);

		vec3 F = FresnelSchlick(HdotV, F0);
		float D = NormalDistributionGGX(normal, halfway, roughness);
		float G = GeometrySmith(roughness, NdotL, NdotV);
		vec3 numerator = F * D * G;
		float denominator = max(4 * NdotL * NdotV, 0.0f) + 0.001f;
		vec3 specular = numerator / denominator;

		// diffuse
		vec3 kD = (vec3(1.0f) - F) * (1 - metallic);
		vec3 diffuse = kD * albedo / PI;

		// Add the light's radiance to the irradiance sum
		spotLightIrradiance += (diffuse + specular) * radiance * NdotL;
	}

	return spotLightIrradiance;
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
	return  F0 + (1.0f - F0) * pow(invcCosTheta, 5.0);
}
highp float rand_1to1(highp float x ) { 
  // -1 -1
  return fract(sin(x)*10000.0);
}

highp float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

void poissonDiskSamples( const in vec2 randomSeed ) {

  float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
  float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

  float angle = rand_2to1( randomSeed ) * PI2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}

void uniformDiskSamples( const in vec2 randomSeed ) {

  float randNum = rand_2to1(randomSeed);
  float sampleX = rand_1to1( randNum ) ;
  float sampleY = rand_1to1( sampleX ) ;

  float angle = sampleX * PI2;
  float radius = sqrt(sampleY);

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec2( radius * cos(angle) , radius * sin(angle)  );

    sampleX = rand_1to1( sampleY ) ;
    sampleY = rand_1to1( sampleX ) ;

    angle = sampleX * PI2;
    radius = sqrt(sampleY);
  }
}

float findBlocker(vec2 uv, float zReceiver ) {
  // set random number
  vec2 seed = texCoords;
  poissonDiskSamples(seed);
  //record bloker number and depth
  int blokerCount = 0;
  float blokerDepthSum = 0.0;
  for (int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES;i++){
    float shadowMapDepth = texture(shadowMap, uv + poissonDisk[i] / textureResolution).r;
    if (shadowMapDepth < zReceiver){
      blokerCount++;
      blokerDepthSum += shadowMapDepth;
    }
  }
  //avoid dividing by zero
  if (blokerCount == 0) return 1.0; 

	return blokerDepthSum/float(blokerCount);
}

float PCF(vec3 coords, float filtersize) {

  //set random number
  vec2 seed = texCoords;
  poissonDiskSamples(seed);

  //calculate visibility
  float visibility = 0.0;
  for (int i = 0; i < PCF_NUM_SAMPLES; i++){
    vec2 offset = poissonDisk[i] * filtersize / textureResolution;
    vec2 offsetCoord = coords.xy + offset;
    float shadowDepth = texture(shadowMap, offsetCoord).r;
    visibility += (coords.z < shadowDepth) ? 1.0:0.0;
  }
  visibility /= float(PCF_NUM_SAMPLES);
  if (visibility < 1e-3) return 0.0;
  return visibility;
}

float PCSS(vec3 coords)
{
	// STEP 1: avgblocker depth
	float zBlocker = findBlocker(coords.xy, coords.z);
	// STEP 2: penumbra size
	float penumbra = (coords.z -  zBlocker)/ zBlocker * LIGHT_WIDTH;
	// STEP 3: filtering
	return PCF(coords, penumbra);

}

float ShadowCalculation(vec3 coords)
{
	float shadow = 0.0;
	//ivec2 textureSize ( gsampler2D sampler, int lod);
	vec2 texelSize = 1.0 /textureResolution;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, coords.xy + vec2(x, y) * texelSize).r; 
			shadow += coords.z > pcfDepth ? 1.0 : 0.0;        
		}    
	}
   shadow /= 9.0;
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
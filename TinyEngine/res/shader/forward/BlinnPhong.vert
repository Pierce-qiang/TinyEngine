#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec2 aTangent;
layout(location = 4) in vec2 aBiTangent;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform mat3 normalMatrix;

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = normalize(normalMatrix * aNormal);
	texCoords = aTexCoord;
	fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
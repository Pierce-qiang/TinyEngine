#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;


out vec3 fragPos;
out mat3 TBN;
out vec3 baseNormal; // for the case with out normal mapping
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 B = normalize(normalMatrix * aBiTangent);
	vec3 N = normalize(normalMatrix * aNormal);

	fragPos = vec3(model * vec4(aPos, 1.0));
	TBN = mat3(T,B,N);
	baseNormal = N;
	texCoords = aTexCoord;

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
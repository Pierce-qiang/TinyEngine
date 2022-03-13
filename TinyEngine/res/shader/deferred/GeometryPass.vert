#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

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
	mat4 totalTransform = mat4(0.0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
           	totalTransform = mat4(1.0);
            break;
        }
        totalTransform += finalBonesMatrices[boneIds[i]] * weights[i];
   }

	fragPos = vec3(model * totalTransform * vec4(aPos, 1.0));
	mat3 normalMat = mat3(transpose(inverse(model * totalTransform)));

	vec3 T = normalize(normalMat * aTangent);
	vec3 B = normalize(normalMat * aBiTangent);
	vec3 N = normalize(normalMat * aNormal);

	TBN = mat3(T,B,N);
	baseNormal = N;
	texCoords = aTexCoord;

	gl_Position = projection * view * vec4(fragPos, 1.0f);
}
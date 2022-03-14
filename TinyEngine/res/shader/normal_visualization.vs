#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

out VS_OUT {
    vec3 normal;
} vs_out;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform mat4 view;
uniform mat4 model;

void main()
{

    vec4 totalPosition = vec4(0.0f);
    mat4 totalTransform = mat4(0.0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        //vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        //totalPosition += localPosition * weights[i];
        totalTransform += finalBonesMatrices[boneIds[i]] * weights[i];
        //vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
        //totalNormal += localNormal * weights[i];
   }
   totalPosition = totalTransform * vec4(pos,1.0f);

    mat3 normalMatrix = mat3(transpose(inverse(view * model * totalTransform)));
    vs_out.normal = vec3(vec4(normalMatrix * norm, 0.0));
    gl_Position = view * model * totalPosition; 
}
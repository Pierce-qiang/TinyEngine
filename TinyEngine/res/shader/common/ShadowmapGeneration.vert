#version 430 core
layout (location = 0) in vec3 aPos;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    // if skeleton animation
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

    gl_Position = lightSpaceMatrix * model * totalTransform * vec4(aPos, 1.0);
}
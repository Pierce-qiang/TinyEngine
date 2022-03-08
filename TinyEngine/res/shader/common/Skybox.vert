#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	texCoords = aPos;
	// view matrix should remove the translation part outside
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;

}
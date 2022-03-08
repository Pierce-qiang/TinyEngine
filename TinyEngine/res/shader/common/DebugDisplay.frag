#version 430 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D displayTexture;
uniform int displayChannel;

void main()
{             
    if(displayChannel == 1)
        FragColor = vec4(texture(displayTexture, texCoords).rrr, 1.0f);
    else if(displayChannel == 2)
        FragColor = vec4(texture(displayTexture, texCoords).ggg, 1.0f);
    else if(displayChannel == 3)
        FragColor = vec4(texture(displayTexture, texCoords).bbb, 1.0f);
    else if(displayChannel == 4)
        FragColor = vec4(texture(displayTexture, texCoords).rgb, 1.0f);
    else if(displayChannel == 5)
        FragColor = vec4(texture(displayTexture, texCoords).aaa, 1.0f);
}
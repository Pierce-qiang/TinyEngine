#version 430 core

// input and output
in vec2 texCoord;
out vec4 FinalColor;

// gbuffer
uniform sampler2D colorTexture;
uniform sampler2D ssrTexture;


void main()
{
	vec4 baseColor = texture(colorTexture, texCoord);
	vec4 ssrColor = texture(ssrTexture,texCoord);



	vec3 color = vec3(baseColor).rgb;
	// vec3 color = vec3(baseColor + ssrColor).rgb;
	
	FinalColor = vec4(color, 1.0f);
}

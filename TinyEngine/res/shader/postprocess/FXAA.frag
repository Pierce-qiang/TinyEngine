#version 430 core

#define FXAA_REDUCE_MIN (1.0 / 128.0)
#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_SPAN_MAX 8.0

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D inputTexture;
uniform vec2 texelSize;

void main() 
{
	// Samples the texels around and calculate their corresponding luminosity
	vec3 calculateLuma = vec3(0.299, 0.587, 0.114);
	vec3 rgbM  = texture2D(inputTexture, texCoord).xyz;
	vec3 rgbNW = texture2D(inputTexture, texCoord + (vec2(-1.0,-1.0)) * texelSize).xyz;
	vec3 rgbNE = texture2D(inputTexture, texCoord + (vec2(1.0,-1.0)) * texelSize).xyz;
	vec3 rgbSW = texture2D(inputTexture, texCoord + (vec2(-1.0,1.0)) * texelSize).xyz;
	vec3 rgbSE = texture2D(inputTexture, texCoord + (vec2(1.0,1.0)) * texelSize).xyz;

	float lumaM  = dot(rgbM,  calculateLuma);
	float lumaNW = dot(rgbNW, calculateLuma);
	float lumaNE = dot(rgbNE, calculateLuma);
	float lumaSW = dot(rgbSW, calculateLuma);
	float lumaSE = dot(rgbSE, calculateLuma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE))); 

	// Calculate sample direction
	vec2 dir;
	// if there is a big difference in top-down, so this is a horizontal line, we use honrizontal pixel to blur
	// if the horizontal line has jag, we blur it successfully, if no jag we actually just blur it with no difference
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE)); 
	// if there is a big difference in left-right, so this is a vertical line, we use vertical pixel to blur
	// if the vertical line has jag, we blur it successfully, if no jag we actually just blur it with no difference
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * texelSize;

	// Perform the samples and calculate the new texel colour
	vec3 rgbA = 0.5 * (texture2D(inputTexture, texCoord + dir * ((1.0 / 3.0) - 0.5)).xyz + texture2D(inputTexture, texCoord + dir * ((2.0 / 3.0) - 0.5)).xyz);
	vec3 rgbB = rgbA * 0.5 + 0.25 * (texture2D(inputTexture, texCoord + dir * 0.5).xyz + texture2D(inputTexture, texCoord + dir * 0.5).xyz);
	
	float lumaB = dot(rgbB, calculateLuma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax))  // if blur too much in rgbB, we use rgbA
		FragColor = vec4(rgbA, 1.0);
	else 
		FragColor = vec4(rgbB, 1.0);
}

#version 430 core

// input and output
in vec2 texCoord;
out vec4 SSRColor;

// gbuffer
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D colorTexture;
uniform sampler2D mixtureTexture;
// uniform sampler2D ssaoTexture;

uniform vec3 viewPos;
uniform mat4 viewMatrix;
uniform mat4 invViewMatrix;
uniform mat4 projectionMatrix;

uniform bool enabled;


void main()
{
	if(!enabled) 
	{
		SSRColor = vec4(0.0);
		return;
	}

	// ssr parameters
  	float maxDistance = 50.0;
	float resolution  = 0.1;
	int   steps       = 10;
	float thickness   = 1.2;

  	vec4 uv = vec4(0.0); // the final use uv coords
  	vec2 texSize  = textureSize(positionTexture, 0).xy;
	vec4 worldPostionFrom = texture(positionTexture, texCoord);

 	vec3 unitWorldPositionFrom = normalize(worldPostionFrom.xyz - viewPos);//V to frag
 	vec3 worldNormal           = normalize(texture(normalTexture, texCoord).xyz);
  	vec3 pivot            	   = normalize(reflect(unitWorldPositionFrom, worldNormal));//R

	// initialize startpoint and endpoint in world space
	vec4 startPoint = vec4(worldPostionFrom.xyz + (pivot *         0.0), 1.0);
  	vec4 endPoint   = vec4(worldPostionFrom.xyz + (pivot * maxDistance), 1.0);
	
	// initialize startpoint and endpoint in screen space, and z-value in view space
	vec4 startFrag = startPoint;
	startFrag      = projectionMatrix * viewMatrix * startFrag;
	startFrag.xyz /= startFrag.w;
	startFrag.xy   = startFrag.xy * 0.5 + 0.5;
	startFrag.xy  *= texSize;
	float startViewDistance = (viewMatrix * startPoint).z;
  	vec4 endFrag = endPoint;
	endFrag      = projectionMatrix * viewMatrix * endFrag;
	endFrag.xyz /= endFrag.w;
	endFrag.xy   = endFrag.xy * 0.5 + 0.5;
	endFrag.xy  *= texSize;
	float endViewDistance = (viewMatrix * endPoint).z;//here z is negative
	if(endViewDistance >= 0)//end frag out of screen
	{
		SSRColor = vec4(0.0);
		return;
	}

	// calculate increment fragment per step
	float deltaX    = endFrag.x - startFrag.x;
	float deltaY    = endFrag.y - startFrag.y;
	float useX      = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
	float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
	vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);//now here is (10, 10 * slope)

	// initialize ssr state parameters
	float search0 = 0; // last miss hit portion [0,1]
  	float search1 = 0; // current hit portion [0,1]
	int hit0 = 0;	   // is hit at first coarse pass
	int hit1 = 0;      // is hit at second refinement pass

	// initialize first step fragment, uv and view space z-value
	vec2 frag  = startFrag.xy;//uv* resolution
    uv.xy = frag / texSize;//change to [0 -1]
	float viewDistance = startViewDistance;//remember negative
	vec4 viewPositionTo = viewMatrix * startPoint;

	// start to ray marching
	float i = 0;
	float depth = thickness;
	for (i = 0; i < 50; ++i) 
	{
		frag      += increment;
		uv.xy      = frag / texSize;
		viewPositionTo = viewMatrix * texture(positionTexture, uv.xy);

		search1 = mix( (frag.y - startFrag.y) / deltaY
					 , (frag.x - startFrag.x) / deltaX, useX);//factor on x 
		search1 = clamp(search1, 0.0, 1.0);

		// perspective correct interpolation for z-value in view space
		// be careful here that the smaller z-value means more away from camera
		viewDistance = (startViewDistance * endViewDistance) / 
						mix(endViewDistance, startViewDistance, search1);
		depth        = viewPositionTo.z - viewDistance;//if depth>0 intersect

		if (depth > 0 && depth < thickness) 
		{
			hit0 = 1;
			break;
		} 
		else 
			search0 = search1;
		
	}

	// refinement the hit point if it exists
	search1 = search0 + ((search1 - search0) / 2.0);//binary search
	steps *= hit0;//
	for (i = 0; i < steps; ++i)
	{
		frag       = mix(startFrag.xy, endFrag.xy, search1);
		uv.xy      = frag / texSize;
		viewPositionTo = viewMatrix * texture(positionTexture, uv.xy);

		viewDistance = (startViewDistance * endViewDistance) / 
						mix(endViewDistance, startViewDistance, search1);
		depth        = viewPositionTo.z - viewDistance;

		if (depth > 0 && depth < thickness) 
		{
			// hit in refinement pass, go closer
			hit1 = 1;
			search1 = search0 + ((search1 - search0) / 2);
		} 
		else 
		{
			// miss in refinement passl go further
			float temp = search1;
			search1 = search1 + ((search1 - search0) / 2);
			search0 = temp;
		}
	}

	vec4 wolrdPositionTo = invViewMatrix * viewPositionTo;
	// TODO: we don't take care the alpha channel now
	float visibility = hit1
		// when reflect ray is point to camera, we fade it out for hit test fail case
		// see more at https://github.com/lettier/3d-game-shaders-for-beginners/blob/master/sections/screen-space-reflection.md
		* (1- max(dot(-unitWorldPositionFrom, pivot), 0))//diff between V and R
		* (1- clamp( depth / thickness, 0, 1))//accuracy close level
		* (1- clamp(length(wolrdPositionTo - worldPostionFrom)/ maxDistance, 0, 1))//distance close level
		* (uv.x < 0 || uv.x > 1 ? 0 : 1) //ensure in screen
		* (uv.y < 0 || uv.y > 1 ? 0 : 1);

	visibility = clamp(visibility, 0, 1);
	SSRColor = vec4(texture(colorTexture, uv.xy).rgb * visibility, 1.0);

	// SSRColor += vec4(texture(colorTexture, texCoord).rgb, 1.0f);
}

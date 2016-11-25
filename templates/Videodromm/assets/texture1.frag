#version 150
// texture1.frag for input texture 1

// uniforms begin
uniform vec3        iResolution;      	// viewport resolution (in pixels)
uniform sampler2D   iChannel0;        	// input channel 0 
uniform sampler2D   iChannel1;        	// input channel 1 
uniform bool        iFlipH;         	// flip horizontally
uniform bool        iFlipV;         	// flip vertically
uniform float       iZoom;              // zoom


out vec4 fragColor;

void main(void)	
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy * iZoom;
	// flip horizontally
	if (iFlipH)
	{
		uv.x = 1.0 - uv.x;
	}
	// flip vertically
	if (iFlipV)
	{
		uv.y = 1.0 - uv.y;
	}
   	vec4 tex = texture(iChannel1, uv);
   	fragColor = vec4( tex.r, tex.g, tex.b, 1.0);
}

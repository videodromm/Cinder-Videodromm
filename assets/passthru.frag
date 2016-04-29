#version 150

uniform vec3        iResolution;         	// viewport resolution (in pixels)
uniform sampler2D   iChannel0;				// input channel 0 (TODO: support samplerCube)
uniform vec3      	iChannelResolution[4]; 	// channel resolution (in pixels)
uniform float       iZoom;               	// zoom

out vec4 oColor;

void main( void )
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	vec4 tex = texture(iChannel0, uv);
	 
	oColor 		= tex;
}

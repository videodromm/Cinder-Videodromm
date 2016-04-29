#version 150
uniform vec3        iResolution;         	// viewport resolution (in pixels)
uniform sampler2D   iChannel0;				// input channel 0 (TODO: support samplerCube)
uniform vec3      	iChannelResolution[4]; 	// channel resolution (in pixels)
uniform float       iZoom;               	// zoom

out vec4 oColor;

void main( void )
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	// zoom centered
	float xZ = (uv.x - 0.5)*iZoom*2.0;
	float yZ = (uv.y - 0.5)*iZoom*2.0;
	vec2 cZ = vec2(xZ, yZ);

	vec4 tex = texture(iChannel0, vec2(cZ.x-iChannelResolution[0].x,cZ.y-iChannelResolution[0].y));
	 
	oColor 		= tex;
}

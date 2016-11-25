#version 150
// shadertoy specific
uniform vec3      	iResolution; 			// viewport resolution (in pixels)
uniform float     	iGlobalTime; 			// shader playback time (in seconds)
uniform vec4      	iMouse; 				// mouse pixel coords. xy: current (if MLB down), zw: click
uniform sampler2D 	iChannel0; 				// input channel 0
uniform sampler2D 	iChannel1; 				// input channel 1
uniform sampler2D 	iChannel2; 				// input channel 2
uniform sampler2D 	iChannel3; 				// input channel 3
uniform vec4      	iDate; 					// (year, month, day, time in seconds)
// videodromm specific
uniform vec3        iBackgroundColor;    	// background color
uniform vec3        iColor;              	// color
uniform int         iSteps;              	// steps for iterations
uniform float       iRatio;
uniform vec2        iRenderXY;           	// move x y
uniform float       iZoom;               	// zoom
uniform float       iCrossfade;          	// CrossFade 2 shaders
uniform float       iAlpha;          	  	// alpha
uniform bool        iFlipH;					// flip horizontally
uniform bool        iFlipV;					// flip vertically
uniform float       iFreq0;              // sound
uniform float       iFreq1;              // sound
uniform float       iFreq2;              // sound
uniform float       iFreq3;              // sound
uniform vec3 		spectrum;              // sound
uniform int         iInvert;           		// 1 for color inversion
uniform float		iFps;

out vec4 fragColor;
vec2  fragCoord = gl_FragCoord.xy; // keep the 2 spaces between vec2 and fragCoord
float time = iGlobalTime;

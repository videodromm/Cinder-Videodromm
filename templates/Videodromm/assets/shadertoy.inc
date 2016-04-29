#version 150
// shadertoy specific
uniform vec3      	iResolution; 			// viewport resolution (in pixels)
uniform float     	iGlobalTime; 			// shader playback time (in seconds)
uniform float     	iChannelTime[4]; 		// channel playback time (in seconds)
uniform vec3      	iChannelResolution[4]; 	// channel resolution (in pixels)
uniform vec4      	iMouse; 				// mouse pixel coords. xy: current (if MLB down), zw: click
uniform sampler2D 	iChannel0; 				// input channel 0 (TODO: support samplerCube)
uniform sampler2D 	iChannel1; 				// input channel 1 
uniform sampler2D 	iChannel2; 				// input channel 2 
uniform sampler2D 	iChannel3; 				// input channel 3 
uniform sampler2D 	iChannel4; 				//  
uniform vec4      	iDate; 					// (year, month, day, time in seconds)
// reymenta specific
uniform sampler2D   iAudio0;				// input channel 0 (audio)
uniform vec3        iBackgroundColor;    	// background color
uniform vec3        iColor;              	// color
uniform int         iSteps;              	// steps for iterations
uniform int         iFade;               	// 1 for fade out
uniform int         iToggle;             	// 1 for toggle
uniform float       iRatio;
uniform vec2        iRenderXY;           	// move x y 
uniform float       iZoom;               	// zoom
uniform int        	iBlendmode;          	// blendmode for channels
uniform float		iRotationSpeed;	  		// Rotation Speed
uniform float       iCrossfade;          	// CrossFade 2 shaders
uniform float       iPixelate;           	// pixelate
uniform int         iGreyScale;          	// 1 for grey scale mode
uniform float       iAlpha;          	  	// alpha
uniform int         iLight;   			  	// 1 for light
uniform int         iLightAuto;          	// 1 for automatic light
uniform float       iExposure;           	// exposure
uniform float       iDeltaTime;          	// delta time between 2 tempo ticks
uniform int         iTransition;   			// transition type
uniform float       iAnim;          		// animation
uniform int         iRepeat;           		// 1 for repetition
uniform int         iVignette;           	// 1 for vignetting
uniform int         iInvert;           		// 1 for color inversion
uniform int         iDebug;           		// 1 to show debug
uniform int         iShowFps;           	// 1 to show fps
uniform float       iFps;          			// frames per second
uniform float       iTempoTime;
uniform int         iGlitch;           		// 1 for glitch
uniform float       iChromatic;				// chromatic if > 0.
uniform float       iTrixels;           	// trixels if > 0.
uniform float       iGridSize;				// gridSize if > 0.
uniform bool        iFlipH;					// flip horizontally
uniform bool        iFlipV;					// flip vertically
uniform int         iBeat;					// measure from ableton
uniform float       iSeed;					// random 
uniform float       iRedMultiplier;			// red multiplier 
uniform float       iGreenMultiplier;		// green multiplier 
uniform float       iBlueMultiplier;		// blue multiplier 
uniform float       iParam1;				// slitscan (or other) Param1
uniform float       iParam2;				// slitscan (or other) Param2 
uniform bool        iXorY;					// slitscan (or other) effect on x or y
uniform float       iBadTv;					// badtv if > 0.

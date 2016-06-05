// https://www.shadertoy.com/view/4sfXRB
void main(void)
{
	vec2 uv = iZoom * gl_FragCoord.xy / iResolution.xy;
	uv.x -= iRenderXY.x;
	uv.y -= iRenderXY.y;
	float time = iGlobalTime;
	float depth = sin(uv.y*2.0+sin(time)*1.5+1.0+sin(uv.x*3.0+time*1.2))*cos(uv.y*2.0+time)+sin((uv.x*3.0+time));
	float texey = (uv.x-0.5);
	float xband = sin(sqrt(uv.y/uv.y)*16.0/(depth)+time*3.0);
	float final = (
		sin(texey/abs(depth)*32.0+time*16.0+sin(uv.y*uv.x*32.0*sin(depth*3.0)))*(depth)*xband
	);

	
	gl_FragColor = vec4(-final*abs(sin(time)),(-final*sin(time)*2.0),(final),1.0)*1.5;
}

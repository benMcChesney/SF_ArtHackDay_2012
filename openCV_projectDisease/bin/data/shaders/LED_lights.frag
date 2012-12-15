#ifdef GL_ES
precision mediump float;
#endif

// Posted by Trisomie21

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

uniform float r ;
uniform float g ;
uniform float b ; 


void main( void ) {
	vec2 v = ( gl_FragCoord.xy / resolution.xy ) ;
	float w = 0.0;
	float x = 0.0;
	float z = 0.0;
	vec2 u;
	vec3 c;
	v*=35.;
	u=floor(v)*.1+vec2(20.,11.);
	u=u*u;
	x=fract(u.x*u.y*9.1+time);
	x*=(1.-length(fract(v)-vec2(.5,.5))*(2.+x));
	c=vec3(v*x,x);
	gl_FragColor = vec4(c.r * r , c.g * g , c.b * b , 1.);
}
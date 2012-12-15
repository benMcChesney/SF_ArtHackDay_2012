#ifdef GL_ES
precision mediump float;
#endif

#define M_PI 3.1415926535897932384626433832795
#define N 7.0

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform float r ;
uniform float g ;
uniform float b ;


void main( void ) {
    
	// This is a reimplementation of this thing:
	// http://mainisusuallyafunction.blogspot.no/2011/10/quasicrystals-as-sums-of-waves-in-plane.html
	
	vec2 position = ( gl_FragCoord.xy ) / 2.0 + mouse * resolution*0.3;
    
	float color = 0.0;
    
	for (float i = 0.0; i < N; ++i) {
		float a = i * (2.0 * M_PI / N);
		color += cos( (position.x * cos(a) + position.y * sin(a)) + time ) / 2.0 + 0.5;
	}
    
	float m = mod(color, 2.0);
	if (m >= 1.0) color = 2.0 - m;
	else color = m;
    
    vec3 color3 = vec3( color ) ; 
	gl_FragColor = vec4( color3.r * r , color3.g * g , color3.b * b , 1.0 );
    
}

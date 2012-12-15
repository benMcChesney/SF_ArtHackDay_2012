#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform float r ;
uniform float g ;
uniform float b ;

mat2 rotMatrix( float r ) {
	return mat2(vec2(cos(r), -sin(r)), vec2(sin(r), cos(r)));
}

vec2 rotate(vec2 v, float r) {
	return v * rotMatrix(r);
}

mat3 transMatrix( vec2 v ) {
	return mat3(vec3(1., 0., v.x), vec3(0., 1., v.y), vec3(0., 0., 1.));
}

vec2 translate(vec2 v, vec2 t) {
	vec3 result = vec3(v.x, v.y, 1.0) * transMatrix(t);
	return vec2(result.x, result.y);
}

vec2 rotateAround( vec2 v, vec2 around, float r) {
	return translate(rotate(translate(v, -around), r), around);
}

void main( void ) {
    
	float time = time;
	
	vec2 position = ( gl_FragCoord.xy / resolution.xy );
	vec2 origin = vec2(0.5, 0.5);
	vec2 right_orig = vec2(0.75, 0.5);
	vec2 left_orig = vec2(0.25, 0.5);
    
	vec3 white = vec3(1., 1., 1.);
	vec3 green = vec3(0.1, 1.0, 0.15 + cos(time) * 0.15);
	vec3 black = vec3(0., 0., 0.);
    
	vec3 color = white;
    
	float dist = distance(position, origin);
	
	
	float dist_right = distance(rotateAround(position, left_orig, cos(dist) * time), right_orig);
	float dist_left = distance(rotateAround(position, right_orig, cos(dist) * time), left_orig);
	
	float time_change = 0.51 + cos(time / 20.) * 0.5;
	
	if (mod(dist, time_change) < time_change / 2.0)
	{
		color = black;
	}
	else
	{
		color = white;
	}
	
	if (mod(dist_right, time_change) < time_change / 2.0)
	{
		color = mix(color, white, 0.5);
	}
	else
	{
		color = mix(color, black, 0.5);
	}
	
	if (mod(dist_left, time_change) < time_change / 2.0)
	{
		color = mix(color, black, 0.5);
	}
	else
	{
		color = mix(color, white, 0.5);
	}
    
	
	
	
	
	
	
	gl_FragColor = vec4( color.r * r , color.g * g , color.b * b , 1.0 );
    
}
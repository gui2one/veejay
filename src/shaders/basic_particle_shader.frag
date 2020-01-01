#version 120

uniform vec3 u_color;
varying float v_opacity;
//~ attribute float opacity;
void main(){
	gl_FragColor = vec4(1.0,1.0,0.0,v_opacity);
}

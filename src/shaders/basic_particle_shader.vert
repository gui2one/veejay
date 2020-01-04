
#version 120

attribute vec3 pos;
attribute float opacity;

varying float v_opacity;

void main(){
	gl_Position = gl_ModelViewMatrix * vec4(pos,1.0);
	v_opacity = opacity;
	
}

#version 120

attribute vec3 pos;
attribute vec2 texcoords;


void main(){
	gl_Position = vec4(pos,1.0);
	gl_TexCoord[0].st = texcoords;
}

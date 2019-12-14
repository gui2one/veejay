#version 120
uniform sampler2D tex;
uniform vec3 u_color;
uniform float u_opacity;
void main(){
	gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * vec4(u_color, u_opacity);
}

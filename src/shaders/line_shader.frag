#version 300 es

layout(location = 0) out mediump vec4 color;
in mediump vec2 f_t_coords;
in mediump vec4 f_color;
uniform sampler2D u_tex; //this is the texture

void main()
{ 
	color = f_color;
}

#version 120
//~ uniform vec4 u_color;


void main()
{           
	gl_Position = gl_ModelViewMatrix * gl_Vertex ;

}



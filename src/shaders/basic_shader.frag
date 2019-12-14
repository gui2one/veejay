#version 120

uniform vec3 u_color;
uniform float u_opacity = 1.0;

void main()
{          
	//~ gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	gl_FragColor = vec4(u_color, u_opacity);
	
}



                                
                                


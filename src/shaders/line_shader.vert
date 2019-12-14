#version 300 es
  
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 t_coords;
uniform vec4 u_color;


out vec4 f_color;
out vec2 f_t_coords;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 u_camera_pos;



void main()
{           
	
	//~ vec4 camera_pos = projection * view * vec4(u_camera_pos, 1.0); //// do not include model matrix in multiplication for the camera
	gl_Position =  projection * view * model * vec4(position, 1.0);
	//~ vec3 camera_dir = normalize(camera_pos.xyz - gl_Position.xyz);
	
	// offset position a small amount towards camera to avoid z fighting, it works pretty well
	//~ gl_Position += vec4(camera_dir * 0.0001,0.0);

	f_color = vec4(color, 1.0);
	//~ f_color *= u_color;
	
	f_t_coords = t_coords;


}



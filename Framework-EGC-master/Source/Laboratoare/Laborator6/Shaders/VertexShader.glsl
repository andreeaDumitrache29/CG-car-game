#version 330

// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_texture;

// Uniform properties
uniform float time;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 obj_color;
uniform vec3 future_color;

// TODO: output values to fragment shader
out vec3 frag_color;
void main()
{	vec3 world_pos = (Model * vec4(v_position,1)).xyz;
	// TODO: send output to fragment shader
	frag_color = obj_color + time * (future_color - obj_color);
	// TODO: compute gl_Position
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}

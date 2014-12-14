#version 330 core

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec4 in_diffuse;

// Model, View, Projection matrix.
uniform mat4 MVP;

out vec4 v2f_color;

void main()
{	
    gl_Position = MVP * vec4(in_position, 1);

	// Just pass the color through directly.
	
	v2f_color = vec4(in_diffuse);
}
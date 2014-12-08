#version 330 core

uniform float time;

layout(location=0) in vec3 in_position;
// layout(location=1) in vec3 in_ambient;

//layout(location=1) in vec3 in_color;

// Model, View, Projection matrix.
uniform mat4 MVP;

out vec4 v2f_color;

void main()
{
	vec4 v = vec4(in_position, 1);
	
    gl_Position = MVP * v;

	// Just pass the color through directly.
    // v2f_color = vec4(in_ambient, 1.0);
	v2f_color = vec4(1.0, 1.0, 1.0, 1.0);
}
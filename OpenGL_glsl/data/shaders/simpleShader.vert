#version 330 core

uniform float time;

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_color;

out vec4 v2f_color;

// Model, View, Projection matrix.
uniform mat4 MVP;

void main()
{
	vec4 v = vec4(in_position, 1);
	
	v.z = sin(5.0 * v.x + time * 0.01) * 0.25;
    gl_Position = MVP * v;

	// Just pass the color through directly.
    v2f_color = vec4(in_color, 1);
}
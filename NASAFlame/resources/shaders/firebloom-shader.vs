#version 330 core
layout (location = 0) in vec3 l_pos;

uniform mat4 projView;
uniform mat4 model;
uniform int blueBloom;

varying vec4 v_worldPos;

void main()
{
	vec4 worldPos = model * vec4(l_pos, 1.0);
	gl_Position = projView * worldPos;

	v_worldPos = worldPos;
}
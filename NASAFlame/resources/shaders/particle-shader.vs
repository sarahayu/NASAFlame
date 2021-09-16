#version 330 core
layout (location = 0) in vec2 l_pos;
layout (location = 1) in vec3 l_offset;

uniform mat4 projView;
uniform mat4 viewRot;
uniform float textureScale;

varying vec2 v_texturePos;

void main()
{
	gl_Position = projView * (transpose(viewRot) * vec4(l_pos, 0.0, 1.0) + vec4(l_offset, 0.0));
	v_texturePos = l_pos.xy + vec2(0.5, 0.5);
}
#version 330 core
layout (location = 0) in vec2 l_pos;

varying vec2 v_texturePos;

void main()
{
	gl_Position = vec4(l_pos * 2, 0.0, 1.0);
	v_texturePos = l_pos.xy + vec2(0.5, 0.5);
}
#version 330 core
out vec4 FragColor;

uniform sampler2D screenTexture;

varying vec2 v_texturePos;

void main()
{
	FragColor = texture(screenTexture, v_texturePos);
} 
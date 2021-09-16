#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

varying vec2 v_texturePos;

void main()
{
	vec2 centered = v_texturePos.xy - vec2(0.5);
	float intensity = pow(centered.x * centered.x + centered.y * centered.y, 6);
	FragColor = vec4(0.2, 0.3, 0.7, intensity * 0.25);
} 
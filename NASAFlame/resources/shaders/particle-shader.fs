#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

varying vec2 v_texturePos;

void main()
{/*
	vec4 color = texture(leafTexture, v_texturePos);
	if (color.a < 0.1) discard;*/
	vec2 centered = v_texturePos.xy - vec2(0.5);
	float dist = sqrt(centered.x * centered.x + centered.y * centered.y);
	float intensity = max(1.0 - dist / 0.5, 0);
	if (dist < 0.1) discard;
	FragColor = vec4(1.0, 222.0 / 255, 63.0 / 255, pow(intensity, 0.5) * (1 - min(1, gl_FragCoord.z / gl_FragCoord.w / 100)));
} 
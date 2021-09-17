#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

varying vec2 v_texturePos;
varying float v_strength;
varying float v_seed;
varying float v_minSoot;

void main()
{
	vec3 colorA = vec3(1.0, 222.0 / 255, 63.0 / 255);
	vec3 colorB = vec3(1.0, 0.0, 0.0);

	vec2 centered = v_texturePos.xy - vec2(0.5);
	float distNormSq = (centered.x * centered.x + centered.y * centered.y) / 0.25;
	if (distNormSq > 1) discard;
	float distNorm = sqrt(clamp(distNormSq, 0, 1));

	float glowTaper = pow(1.0 - max(distNorm - 0.8, 0) / 0.2, 1);
	float sootVisibility = min(pow(1.3 * distNorm, 4) + v_strength, 1.0);

	float ambientFog = (1 - clamp(max(0, (gl_FragCoord.z / gl_FragCoord.w - 80)) / 50, 0, 1));
	vec3 flameColor = mix(colorA, colorB, pow(1 - v_strength, 0.6)) * (0.7 + 0.3 * v_strength);
	vec3 finalColor;
	if (v_seed < v_minSoot)
	{
		vec3 sootAndFlameColor = mix(vec3(0), flameColor, sootVisibility);
		finalColor = mix(vec3(0), sootAndFlameColor, ambientFog);
	}
	else
		finalColor = mix(vec3(0), flameColor, ambientFog);

	FragColor = vec4(finalColor, glowTaper);
} 
#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;
uniform float fogOffset;
uniform float fogFactor;

varying vec2 v_texturePos;
varying float v_strength;
varying float v_seed;
varying float v_minSoot;

void main()
{
	vec3 colorA = vec3(1.0, 222.0 / 255, 63.0 / 255);
	vec3 colorB = vec3(1.0, 0.0, 0.0);
	float ambientFog = (1 - clamp(max(0, (gl_FragCoord.z / gl_FragCoord.w - fogOffset)) / fogFactor, 0, 1));
	vec3 flameColor = mix(colorA, colorB, pow(1 - v_strength, 0.6)) * (0.7 + 0.3 * v_strength);

	vec2 centered = v_texturePos.xy - vec2(1.0);
	float distNorm = sqrt(centered.x * centered.x + centered.y * centered.y) / 0.5;
	float blurFactor = min(sqrt(pow(abs(gl_FragCoord.x - 300) / 300, 2) + pow(abs(gl_FragCoord.y - 300) / 300, 2)), 1);
	blurFactor = pow(blurFactor, 2);
	float blurDimFactor = mix(1, 0.75, blurFactor);
	if (distNorm > 2) discard;
	float blurDistNorm = pow(distNorm, 2);

	if (blurDistNorm < 1)
	{
		float glowTaper = pow(1.0 - max(distNorm - 0.8, 0) / 0.2, 1);
		float sootVisibility = min(pow(1.3 * distNorm, mix(4, 0.5, blurFactor)) + v_strength, 1.0);

		vec3 finalColor;
		if (v_seed < v_minSoot)
		{
			vec3 sootAndFlameColor = mix(vec3(0), flameColor, sootVisibility);
			finalColor = mix(vec3(0), sootAndFlameColor, ambientFog);
		}
		else
			finalColor = mix(vec3(0), flameColor, ambientFog);

		FragColor = vec4(finalColor, 1.0);
	}
	else
	{
		vec3 finalColor = mix(vec3(0), flameColor, ambientFog);
		blurDistNorm = blurDistNorm - 1;
		blurDistNorm *= mix(3, 1, blurFactor);
		if (blurDistNorm > 1) discard;
		blurDistNorm = 1 - blurDistNorm;
		FragColor = vec4(finalColor, pow(blurDistNorm, mix(1, 1, blurFactor)));
	}
} 
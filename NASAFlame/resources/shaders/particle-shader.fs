#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;
uniform float fogOffset;
uniform float fogFactor;
uniform float timeOffset;

varying vec2 v_texturePos;
varying float v_strength;
varying float v_seed;
varying float v_minSoot;
varying float v_blurAllowance;

float revlerp(float num, float minn, float maxx)
{
	return clamp(num - minn, 0, maxx) / (maxx - minn);
}

void main()
{
	vec3 colorA = vec3(2.0, 255.0 / 255, 63.0 / 255);
	vec3 colorB = vec3(2.0, 0.0, 0.0);

	float timeOffsetActual = max(0, 2 * (timeOffset - 0.5));
	float lifespanLeft = clamp(v_strength - timeOffsetActual, 0, 1);//max(0, v_strength - max(0, 2 * (timeOffsetActual - 0.5)));

	vec2 centered = v_texturePos.xy;
	float distNormSq = centered.x * centered.x + centered.y * centered.y;
	if (distNormSq > 1) discard;
	float distNorm = sqrt(distNormSq) * v_blurAllowance;

	float age = 1 - lifespanLeft;
	float lifespanFactorOnColor = pow(age, mix(0.4, 1.0, pow(v_strength, 4)));
	vec3 flameColor = mix(colorA, colorB, lifespanFactorOnColor) * mix(0.7, 1.0, lifespanLeft);
	// as particles get further away from screen, fade to black
	float ambientFog = clamp(max(0, (gl_FragCoord.z / gl_FragCoord.w - fogOffset)) / fogFactor, 0, 1);

	// raw color without depth of field applied
	vec4 clearColor = vec4(0.0);

	// how much to blur particle based on distance from screen; minus 0.08 so focus is just beyond the front of flame sphere
	float blurFactor = abs(pow(ambientFog, 2) - 0.08);
	// modified distNorm to enlarge particle dimensions to make it look more natural to blur into
	float blurredDistNorm = distNorm / mix(1.0, v_blurAllowance, blurFactor);

	float disappearAhead = 0.3;

	if (blurredDistNorm < 1)
	{
		// if blurFactor is large, make taper more gradual. Otherwise, make it sharper
		float glowTaper = pow(1.0 - revlerp(blurredDistNorm, mix(0, 0.8, 1 - blurFactor), 1.0), 1);

		vec3 finalColor;
		if (v_seed < v_minSoot)
		{
			disappearAhead = 0.0;
			//float invSootVisibility = min(pow(1.3 * blurredDistNorm, 4) + 0, 1.0);
			float invSootVisibility = max(0.0, min(pow(1.3 * blurredDistNorm, 4) + v_strength / 2, 1.0) - timeOffsetActual);
			vec3 sootAndFlameColor = mix(vec3(0), flameColor * 2, invSootVisibility);
			finalColor = mix(sootAndFlameColor, vec3(0), ambientFog);
		}
		else
			finalColor = mix(flameColor, vec3(0), ambientFog);
		clearColor = vec4(finalColor, glowTaper);
	}

	// pure blurred particle color, just make it a plain radial gradient
	vec4 blurColor = vec4(mix(flameColor, vec3(0), ambientFog), min(0.2, 1.0 - (distNorm / v_blurAllowance)));

	// mix blurred and clear color by blurFactor
	vec4 final = mix(clearColor, blurColor, blurFactor);

	// flicker
	float offset = (timeOffset + v_strength) * 5.0;
	final.a *= mix(0.2, 1.1, (sin(offset) + sin(offset * 4.5) + sin(offset * 7) + 3) / 6);

	// make particle completely transparent if at the end of the lifespan
	final.a *= pow(clamp(5 * (lifespanLeft - disappearAhead), 0, 1), 1);

	// final tweak, blurred objects tend to be quite transparent and a little desaturated?
	final.a *= pow(1.0 - (distNorm / v_blurAllowance), 2.5) * mix(0.5, 1.0, 1 - blurFactor);
	final.rgb *= mix(0.85, 1.0, pow(1.0 - blurFactor, 10));

	if (final.a < 0.03) discard;

	FragColor = final;

} 
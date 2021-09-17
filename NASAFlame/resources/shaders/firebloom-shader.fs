#version 330 core
out vec4 FragColor;

uniform sampler2D leafTexture;

varying vec2 v_texturePos;

void main()
{
	vec2 centered = v_texturePos - vec2(0.5);
	float distSq = centered.x * centered.x + centered.y * centered.y;
	float blueIntensity = pow(distSq, 10);
	float orangeRad = distSq + 0.14;
	float orangeIntensity = orangeRad > 1 ? 0 : orangeRad;
	orangeIntensity = pow(orangeIntensity, 0.8);
	orangeIntensity *= pow(1 - clamp(abs(centered.y) / 0.5, 0, 1), 0.5);

	vec3 orangeColor = vec3(255.0 / 255, 80.0 / 255, 10.0 / 255);

	vec4 orangeFlame = vec4(orangeColor, orangeIntensity);
	vec4 blueFlame = vec4(0.2, 0.3, 0.7, blueIntensity);
	vec4 finalColor = vec4(vec3(orangeFlame * orangeFlame.a * (1 - blueFlame.a) + blueFlame * blueFlame.a), orangeIntensity * 0.4 + blueIntensity * 0.2);
	
	FragColor = finalColor;
} 
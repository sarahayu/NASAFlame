#version 330 core
out vec4 FragColor;

uniform mat4 projView;
uniform mat4 model;
uniform int blueBloom;
uniform float timeOffset;

varying vec4 v_worldPos;
varying float v_thickness;

void main()
{
	float timeActual = timeOffset;
	if (blueBloom == 1)
		FragColor = vec4(vec3(0.2, 0.3, 0.7) * (1 + v_thickness), 0.15 * v_thickness);
	else
	{
		
		FragColor = vec4(vec3(255.0 / 255, 67.0 / 255, 10.0 / 255) * (1 + v_thickness), 0.7 * v_thickness);
		timeActual += 0.2;
	}

	FragColor.a *= pow(clamp(5 * (1 - timeActual), 0, 1), 1);
} 
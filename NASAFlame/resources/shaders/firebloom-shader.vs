#version 330 core
layout (location = 0) in vec3 l_pos;

uniform mat4 projView;
uniform mat4 model;
uniform int blueBloom;
uniform vec3 cameraPos;
uniform float blueBloomOffset = 1.5f;
uniform float heatShieldThickness = 3.f;
uniform float timeOffset;

varying vec4 v_worldPos;
varying float v_thickness;

void main()
{
	float timeActual = pow(timeOffset, 0.4) * 1.1;
	mat4 scaledModel = model;
	if (blueBloom == 0) scaledModel[1][1] *= 0.9;

	vec4 worldPos = scaledModel * vec4(l_pos * timeActual, 1.0);
	gl_Position = projView * worldPos;

	float radius = scaledModel[0][0] * timeActual;

	vec3 ray = normalize(worldPos.xyz - cameraPos);
	float a = ray.x * ray.x + ray.y * ray.y + ray.z * ray.z;
	float b = (ray.x * cameraPos.x + ray.y * cameraPos.y + ray.z * cameraPos.z) * 2;
	float c = cameraPos.x * cameraPos.x + cameraPos.y * cameraPos.y + cameraPos.z * cameraPos.z - radius * radius;
	float discriminant = sqrt(b * b - 4 * a * c);
	float t1 = (-b + discriminant) / (2 * a);
	float t2 = (-b - discriminant) / (2 * a);
	vec3 p1 = vec3(cameraPos.x + ray.x * t1, cameraPos.y + ray.y * t1, cameraPos.z + ray.z * t1);
	vec3 p2 = vec3(cameraPos.x + ray.x * t2, cameraPos.y + ray.y * t2, cameraPos.z + ray.z * t2);

	float smallerRadius = radius - 3.f;
	c = cameraPos.x * cameraPos.x + cameraPos.y * cameraPos.y + cameraPos.z * cameraPos.z - smallerRadius * smallerRadius;
	bool noPassThrough = (b * b - 4 * a * c) < 0;
	
	if (noPassThrough)
	{
		float thickness = distance(p1, p2);
		v_thickness = (thickness) / (radius * 2);
	}
	else
	{
		discriminant = sqrt(b * b - 4 * a * c);
		t1 = (-b + discriminant) / (2 * a);
		t2 = (-b - discriminant) / (2 * a);
		vec3 p3 = vec3(cameraPos.x + ray.x * t1, cameraPos.y + ray.y * t1, cameraPos.z + ray.z * t1);
		vec3 p4 = vec3(cameraPos.x + ray.x * t2, cameraPos.y + ray.y * t2, cameraPos.z + ray.z * t2);

		float rad2 = radius/2;
		float maxThickness = sqrt(radius * radius - rad2 * rad2);

		float thickness = distance(p1, p3) + distance(p2, p4);

		v_thickness = (thickness) / (radius * 2);
	}

	if (blueBloom == 0) v_thickness *= pow(1 - abs(worldPos.y) / scaledModel[1][1], 2);

	v_worldPos = worldPos;
}
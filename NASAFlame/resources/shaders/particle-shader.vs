#version 330 core
layout (location = 0) in vec2 l_pos;
layout (location = 1) in vec3 l_offset;
layout (location = 2) in float l_strength;
layout (location = 3) in float l_seed;

uniform mat4 projView;
uniform mat4 viewRot;
uniform float minSoot;
uniform float textureScale;

varying vec2 v_texturePos;
varying float v_strength;
varying float v_seed;
varying float v_minSoot;

void main()
{
	float strengthSize = l_seed < minSoot ? 0.6 + 0.4 * l_strength : 0.1 + 0.2 * l_strength;
	gl_Position = projView * (transpose(viewRot) * vec4(l_pos * strengthSize * 2, 0.0, 1.0) + vec4(l_offset, 0.0));
	v_texturePos = (l_pos.xy + vec2(0.5, 0.5)) * 2;
	v_strength = l_strength;
	v_seed = l_seed;
	v_minSoot = minSoot;
}
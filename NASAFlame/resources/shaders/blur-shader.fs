#version 330 core
out vec4 FragColor;

uniform sampler2D screenTexture;  
uniform bool horizontal;
uniform int intensity;

uniform float weight[4] = float[] (0.1945946, 0.1216216, 0.054054, 0.016216);

varying vec2 v_texturePos;

void main()
{             
    vec2 textureOffset = 1.0 / textureSize(screenTexture, 0);
    vec3 result = texture(screenTexture, v_texturePos).rgb * weight[0];
    if(horizontal)
    {
        for(int i = 0; i < 4; i++)
        {
            result += texture(screenTexture, v_texturePos + vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(screenTexture, v_texturePos - vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 0; i < 4; i++)
        {
            result += texture(screenTexture, v_texturePos + vec2(0.0, textureOffset.y * i)).rgb * weight[i];
            result += texture(screenTexture, v_texturePos - vec2(0.0, textureOffset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
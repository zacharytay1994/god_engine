#version 430 core

out vec4 fFragColor;

in vec2 vTexCoord;

uniform sampler2D uImage;

uniform bool uHorizontal;
uniform float uWeight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{           
    vec2 tex_offset = 1.0 / textureSize(uImage, 0); // gets size of single texel
    vec3 result = texture(uImage, vTexCoord).rgb * uWeight[0]; // current fragment's contribution
    if(uHorizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uImage, vTexCoord + vec2(tex_offset.x * i, 0.0)).rgb * uWeight[i];
            result += texture(uImage, vTexCoord - vec2(tex_offset.x * i, 0.0)).rgb * uWeight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(uImage, vTexCoord + vec2(0.0, tex_offset.y * i)).rgb * uWeight[i];
            result += texture(uImage, vTexCoord - vec2(0.0, tex_offset.y * i)).rgb * uWeight[i];
        }
    }
    fFragColor = vec4(result, 1.0);
}
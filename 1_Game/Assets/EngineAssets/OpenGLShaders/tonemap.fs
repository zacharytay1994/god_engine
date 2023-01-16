#version 430 core

out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uHdrBuffer;

void main()
{
    // reinhart tonemapping        
    // const float gamma = 1.0;
    vec3 hdrColor = texture(uHdrBuffer, vTexCoord).rgb;
  
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction 
    // mapped = pow(mapped, vec3(1.0 / 2.2));
  
    FragColor = vec4(mapped, 1.0);
}
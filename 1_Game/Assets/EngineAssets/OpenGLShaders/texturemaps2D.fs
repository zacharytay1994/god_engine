#version 430 core

// out vec4 fFragColor;
layout (location = 0) out vec4 fFragColor;

in vec2 vUV;

// material properties
uniform sampler2D diffuse_map;
uniform float uAlpha;

void main()
{ 
   fFragColor = texture(diffuse_map, vUV);
   // gamma correction, gamma 2.2
   // fFragColor.rgb = pow(fFragColor.rgb, vec3(1.0/2.2));
   fFragColor.rgb = fFragColor.rgb;
   fFragColor.a = fFragColor.a * uAlpha;
}


#version 430 core

// out vec4 fFragColor;
layout (location = 0) out vec4 fFragColor;

in vec2 vUV;

// material properties
uniform sampler2D diffuse_map;

void main()
{ 
   // fFragColor = texture(diffuse_map, vUV);
   // // gamma correction, gamma 2.2
   // fFragColor.rgb = pow(fFragColor.rgb, vec3(1.0/2.2));

   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(diffuse_map, vUV).r);
   fFragColor.rgba = vec4(1.0,0.0,1.0,1.0) * sampled;
   fFragColor.rgb = pow(fFragColor.rgb, vec3(1.0/2.2));
}


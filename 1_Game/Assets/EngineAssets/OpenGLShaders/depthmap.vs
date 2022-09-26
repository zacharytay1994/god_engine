#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 aModel;

uniform mat4 uLightSpaceMatrix;

void main() 
{
	gl_Position = uLightSpaceMatrix * aModel * vec4(aPos,1.0);
}
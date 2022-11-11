#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec4 aColour;
layout (location = 5) in mat4 aModel;

out vec2 vUV;
uniform mat4 uProjection;

void main() 
{
	vUV = aUV;
	gl_Position = uProjection * aModel * vec4(aPos, 1.0);
}
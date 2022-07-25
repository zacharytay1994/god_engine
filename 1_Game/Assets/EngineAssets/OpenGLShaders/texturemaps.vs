#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec4 aColour;

out vec2 vUV;
out vec3 vNormal;
out vec3 vWorldPos;

uniform mat4 uModel;
uniform mat4 uProjection;
uniform mat4 uView;

void main() 
{
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
	vUV = aUV;
	vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vWorldPos = vec3(uModel * vec4(aPos, 1.0));
}
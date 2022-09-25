#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec4 aColour;
layout (location = 5) in mat4 aModel;

out vec2 vUV;
out vec3 vNormal;
out vec3 vWorldPos;
out vec4 vFragPosLightSpace;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uLightSpaceMatrix;

void main() 
{
	gl_Position = uProjection * uView * aModel * vec4(aPos, 1.0);
	vUV = aUV;
	vNormal = mat3(transpose(inverse(aModel))) * aNormal;
    vWorldPos = vec3(aModel * vec4(aPos, 1.0));
	vFragPosLightSpace = uLightSpaceMatrix * vec4(vWorldPos, 1.0f);
}
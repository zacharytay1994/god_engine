#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec4 aColour;
layout (location = 5) in mat4 aModel;

out vec2 vUV;
uniform mat4 uProjection;
uniform vec2 uSpritesheetData = vec2(0,1);

void main() 
{
	// vUV = aUV;
	vUV = vec2( ( uSpritesheetData.x * 1.0 + aUV.x ) / uSpritesheetData.y , aUV.y );
	gl_Position = uProjection * aModel * vec4(aPos, 1.0);
}
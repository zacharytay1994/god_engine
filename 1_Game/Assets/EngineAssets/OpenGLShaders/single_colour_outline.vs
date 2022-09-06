#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec4 aColour;

out vec2 vUV;

uniform mat4 uModel;
uniform mat4 uProjection;
uniform mat4 uView;
uniform float uOutlining;

void main() 
{
    gl_Position = uProjection * uView * uModel * vec4(aPos + aNormal * uOutlining, 1.0);
}
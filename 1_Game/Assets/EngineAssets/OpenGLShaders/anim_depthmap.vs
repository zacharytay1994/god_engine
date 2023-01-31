#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) in ivec4 in_BoneIds;
layout(location = 6) in vec4 in_Weights;

uniform mat4 uLightSpaceMatrix;
uniform mat4 uModel;

uniform mat4 bone_transforms[100];

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

void main() 
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);

    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if(in_BoneIds[i] == -1)
            continue;
        if (in_BoneIds[i] >=MAX_BONES)
        {
            totalPosition = vec4(in_Position,1.0f);
            
            break;
        }
        vec4 localPosition = (bone_transforms[in_BoneIds[i]] * in_Weights[i]) * vec4(in_Position,1.0f);
        totalPosition += localPosition;
        vec3 localNormal =mat3(transpose(inverse(bone_transforms[in_BoneIds[i]]))) * in_Normal;
        totalNormal += localNormal * in_Weights[i];
    }

    // mat4 viewModel = uView * uModel;
    gl_Position = uLightSpaceMatrix * uModel * totalPosition;
	// gl_Position = uLightSpaceMatrix * aModel * vec4(aPos,1.0);
}
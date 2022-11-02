/*
// [1st working version - Currently Not in Use]
#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) in ivec4 in_BoneIds;
layout(location = 6) in vec4 in_Weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
// See kMaxBonesCount.
uniform mat4 bone_transforms[100];

out vec2 v_UV;
out vec3 v_Position;
out mat3 v_TBN;

void main()
{
    mat4 S = mat4(0.f);
    for (int i = 0; i < 4; ++i)
    {
        if (in_BoneIds[i] >= 0)
        {
            S += (bone_transforms[in_BoneIds[i]] * in_Weights[i]);
        }
    }
    mat3 S_ = transpose(inverse(mat3(S)));
    mat4 MVP = projection * view * model;
    gl_Position = MVP * S * vec4(in_Position, 1.f);
    v_UV = in_UV;
    v_Position = vec3(model * vec4(in_Position, 1.f));
    
    vec3 T = normalize(S_ * in_Tangent);
    vec3 B = normalize(S_ * in_Bitangent);
    vec3 N = normalize(S_ * in_Normal);
    v_TBN = mat3(T, B, N);
}
*/

/*
// [2nd working version - Work in Progress / Testing phrase]
#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) in ivec4 in_BoneIds;
layout(location = 6) in vec4 in_Weights;

out vec2 vUV;
out vec3 vNormal;
out vec3 vWorldPos;
out vec4 vFragPosLightSpace;

smooth out vec4 vEyeSpacePosition;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uLightSpaceMatrix;

// See kMaxBonesCount.
uniform mat4 uModel;
uniform mat4 bone_transforms[100];

void main()
{
    mat4 S = mat4(0.f);
    for (int i = 0; i < 4; ++i)
    {
        if (in_BoneIds[i] >= 0)
        {
            S += (bone_transforms[in_BoneIds[i]] * in_Weights[i]);
        }
    }
    mat4 S_ = transpose(inverse(mat4(S)));
    gl_Position = (uProjection * uView * uModel) * S * vec4(in_Position, 1.0);
    vUV = in_UV;
    // vNormal = mat3(transpose(inverse(uModel * S))) * in_Normal;
    // vNormal = normalize(S_  * in_Normal );
    vNormal = mat3(transpose(inverse(uModel * S))) * in_Normal;
    // vNormal = vec3(normalize(S  * vec4(in_Normal,1.0)).xyz);
    vWorldPos =  vec3(uModel * vec4(in_Position, 1.f));
	vFragPosLightSpace = uLightSpaceMatrix * vec4(vWorldPos, 1.0f);
	vEyeSpacePosition = ( uView * uModel ) * vec4(in_Position, 1.0) ;
}
*/

// [3rd working version - Work in Progress / Testing phrase]
#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) in ivec4 in_BoneIds;
layout(location = 6) in vec4 in_Weights;

out vec2 vUV;
out vec3 vNormal;
out vec3 vWorldPos;
out vec4 vFragPosLightSpace;

smooth out vec4 vEyeSpacePosition;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uLightSpaceMatrix;

// See kMaxBonesCount.
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

    mat4 viewModel = uView * uModel;
    gl_Position = uProjection * viewModel * totalPosition ;
    vUV = in_UV;
    vNormal = totalNormal;
    vWorldPos =  vec3(uModel * vec4(in_Position, 1.f));
	vFragPosLightSpace = uLightSpaceMatrix * vec4(vWorldPos, 1.0f);
	vEyeSpacePosition = ( uView * uModel ) * vec4(in_Position, 1.0) ;
}
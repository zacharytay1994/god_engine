// #version 430 core

// layout(location = 0) in vec3 pos;
// layout(location = 1) in vec3 norm;
// layout(location = 2) in vec2 tex;
// layout(location = 3) in vec3 tangent;
// layout(location = 4) in vec3 bitangent;
// layout(location = 5) in ivec4 boneIds; 
// layout(location = 6) in vec4 weights;

// uniform mat4 projection;
// uniform mat4 view;
// uniform mat4 model;

// const int MAX_BONES = 100;
// const int MAX_BONE_INFLUENCE = 4;
// uniform mat4 finalBonesMatrices[MAX_BONES];

// out vec2 TexCoords;

// void main()
// {

//      mat4   BoneTransform  = finalBonesMatrices[boneIds[0]] * weights[0];
//             BoneTransform += finalBonesMatrices[boneIds[1]] * weights[1];
//             BoneTransform += finalBonesMatrices[boneIds[2]] * weights[2];
//             BoneTransform += finalBonesMatrices[boneIds[3]] * weights[3];

//         vec4 boned_position = BoneTransform * vec4(pos, 1.0); 
//         gl_Position =(projection * view * model ) * boned_position;
    
// //     vec4 totalPosition = vec4(0.0f);
// //     for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
// //     {
// //         if(boneIds[i] == -1) 
// //             continue;
// //         if(boneIds[i] >=MAX_BONES) 
// //         {
// //             totalPosition = vec4(pos,1.0f);
// //             break;
// //         }
// //         vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
// //         totalPosition += localPosition * weights[i];
// //         vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
// //    }
	
// //     mat4 viewModel = view * model;
// //     gl_Position =  projection * viewModel * totalPosition;
// // 	TexCoords = tex;
// }

#version 330 core

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
#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inNormal;
layout (location = 4) in vec4 inColour;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec2 fragTexCoord;
layout (location = 2) out mat4 fragModel;

// push constant block
layout ( push_constant ) uniform constants
{
    mat4 model_matrix;
    mat4 render_matrix;
} PushConstant;

void main() {
    gl_Position = PushConstant.render_matrix * vec4(inPosition, 1.0);
    //gl_Position = vec4(inPosition, 1.0);
    fragColor = inColour;
    fragTexCoord = inUV;
    fragModel = PushConstant.model_matrix;
}
#version 450

layout (location = 0) in vec4 fragColour;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in mat4 fragModel;

layout (location = 0) out vec4 outColour;

layout (binding = 0) uniform sampler2D texSampler;
layout (binding = 1) uniform sampler2D normalMapSampler;

void main() {
    vec3 Normal;
	// For BC5 it used (rg)
	Normal.xy	= (texture(normalMapSampler, fragTexCoord).gr * 2.0) - 1.0;
	
	// Derive the final element
	Normal.z =  sqrt(1.0 - dot(Normal.xy, Normal.xy));

    Normal = vec3(fragModel * vec4(Normal,1.0));

    vec3 LightDirection = vec3(0.0f,0.0f,1.0f);

    // Compute the diffuse intensity
	float DiffuseI  = max( 0, dot( Normal, LightDirection ));

    outColour = texture(texSampler, fragTexCoord) * DiffuseI;
}
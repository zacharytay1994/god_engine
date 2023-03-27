#version 430 core

// out vec4 fFragColor;
layout ( location = 0 ) out vec4 fFragColor;
layout ( location = 1 ) out vec4 fBrightColor;

in vec2 vUV;
in vec3 vNormal;
in vec3 vWorldPos;
in vec4 vFragPosLightSpace;

smooth in vec4 vEyeSpacePosition;

uniform vec3 uViewPosition;

// material properties
struct sMaterial
{
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
};

uniform sMaterial uMaterial;

// tint
uniform vec4 uTint;
uniform bool uIsTint = false;

uniform float uEmissive = 1.0f;

void main( )
{
    // pre-read variables
    vec4 output_color = vec4( 0 );
    vec4 diffuse_color = texture( uMaterial.diffuse_map , vUV ) * uTint;
    vec4 specular_color = texture( uMaterial.specular_map , vUV );

    // discard pixels with alpha <= 0.001
    if (diffuse_color.a <= 0.1)
    {
        discard;
    }

    // emissive
    output_color = uEmissive * diffuse_color;

    // render to hdr texture if pass certain threshold
    float brightness = dot( output_color.rgb , vec3( 0.2126 , 0.7152 , 0.0722 ) );
    if ( brightness > 10.0 )
        fBrightColor = vec4( output_color.rgb , diffuse_color.a );
    else
        fBrightColor = vec4( 0.0 , 0.0 , 0.0 , 1.0 );

    fFragColor = diffuse_color;
}

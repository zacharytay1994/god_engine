#version 430 core

out vec4 fFragColor;

in vec2 vTexCoord;

uniform sampler2D uScene;
uniform sampler2D uBloomBlur;
uniform float uExposure = 1.0;

void main( )
{          
    // const float gamma = 2.2;
    vec3 hdrColor = texture( uScene , vTexCoord ).rgb;
    vec3 bloomColor = texture( uBloomBlur , vTexCoord ).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3( 1.0 ) - exp( -hdrColor * uExposure );
    // also gamma correct while we're at it       
    // result = pow(result, vec3(1.0 / gamma));
    fFragColor = vec4( result , 1.0 );
}
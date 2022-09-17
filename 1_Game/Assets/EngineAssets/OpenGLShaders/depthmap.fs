#version 430 core

void main()
{
    // fFragColor =  vec4(LightValue(), 1.0);
    gl_FragDepth = gl_FragCoord.z;
}
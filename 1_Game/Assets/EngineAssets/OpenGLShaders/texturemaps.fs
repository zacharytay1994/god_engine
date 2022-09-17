#version 430 core

out vec4 fFragColor;

in vec2 vUV;
in vec3 vNormal;
in vec3 vWorldPos;
in vec4 vFragPosLightSpace;

uniform vec3 uViewPosition;

// material properties
struct Material 
{
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
};

uniform Material uMaterial;

// light properties
struct Light 
{
    vec3 position;
    vec3 colour;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light uLight;

//skybox
uniform samplerCube uSkybox; //--

// shadowmap
uniform sampler2D uShadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(vNormal, normalize(vec3( -2.0f, 4.0f, -1.0f )))), 0.001);
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x,y)*texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // clamp far plane
    if (projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}

vec3 LightValue()
{
    vec3 normal = normalize(vNormal);
    vec3 light_direction = normalize(uLight.position - vWorldPos);
    vec3 view_direction = normalize(uViewPosition - vWorldPos);
    vec3 reflect_direction = reflect(-light_direction, normal);

    // ambient
    vec4 ambient = vec4((uLight.colour * uLight.ambient), 1.0) * texture(uMaterial.diffuse_map, vUV);

    // diffuse
    float diffuse_scalar = max(dot(normal, light_direction), 0.0);
    vec4 diffuse = vec4((uLight.colour * uLight.diffuse), 1.0) * (diffuse_scalar * texture(uMaterial.diffuse_map, vUV));

    // cubemap reflection
    vec3 I = normalize (vWorldPos - uViewPosition) ; //--
    vec3 R = reflect (I ,normalize(vNormal)) ; //--
    R.z = R.z * -1.0f; //--
    vec4 cubemap_colour =  vec4(texture(uSkybox, R).rgb, 1.0);

    // specular
    float specular_scalar = pow(max(dot(view_direction, reflect_direction), 0.0), uMaterial.shininess);
    vec4 specular = cubemap_colour * (specular_scalar * texture(uMaterial.specular_map, vUV));

    // calculate shadow
    float shadow = ShadowCalculation(vFragPosLightSpace);

    // return vec3 ( ambient + diffuse + specular ) * (1.0-shadow);
    // return (ambient + (1.0 - shadow) * (diffuse + specular)) *colo
    return vec3((ambient + (1.0 - shadow) * (diffuse + specular)) * texture(uMaterial.diffuse_map, vUV));
}

void main()
{
    fFragColor = vec4(LightValue(),1.0);
}
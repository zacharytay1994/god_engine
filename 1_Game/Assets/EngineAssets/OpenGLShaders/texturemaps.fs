#version 430 core

out vec4 fFragColor;

in vec2 vUV;
in vec3 vNormal;
in vec3 vWorldPos;
in vec4 vFragPosLightSpace;

uniform vec3 uViewPosition;

// material properties
struct sMaterial 
{
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
};

uniform sMaterial uMaterial;

// light properties
struct sLight 
{
    vec3 position;
    vec3 colour;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct sDirectionalLight 
{
    vec3 direction;
    vec3 colour;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct sSpotLight 
{
    vec3 direction;
    vec3 position;  
    vec3 viewPos;

    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;


};

uniform sLight              uPointLight;
uniform sDirectionalLight   uDirectionalLight;
uniform sSpotLight          uSpotLight;

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
    // calculate light position
    vec3 light_dir = normalize(vec3(10.0f, 10.0f, 10.0f) - vWorldPos);
    float bias = max(0.05 * (1.0 - dot(vNormal, light_dir)), 0.002);

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

vec3 PointLight()
{
    vec3 normal = normalize(vNormal);
    vec3 light_direction = normalize(uPointLight.position - vWorldPos);
    vec3 view_direction = normalize(uViewPosition - vWorldPos);
    vec3 reflect_direction = reflect(-light_direction, normal);

    // ambient
    vec4 ambient = vec4((uPointLight.colour * uPointLight.ambient), 1.0) * texture(uMaterial.diffuse_map, vUV);

    // diffuse
    float diffuse_scalar = max(dot(normal, light_direction), 0.0);
    vec4 diffuse = vec4((uPointLight.colour * uPointLight.diffuse), 1.0) * (diffuse_scalar * texture(uMaterial.diffuse_map, vUV));

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

    return vec3((ambient + (1.0 - shadow) * (diffuse + specular)) * texture(uMaterial.diffuse_map, vUV));
}


vec3 DirectLight()
{
    // ambient
    vec3 ambient = uDirectionalLight.ambient * texture(uMaterial.diffuse_map, vUV).rgb;

    // diffuse
	vec3 normal = normalize(vNormal);
    vec3 light_direction = normalize(-uDirectionalLight.direction);
    float diffuse_scalar = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = uDirectionalLight.diffuse * diffuse_scalar * texture(uMaterial.diffuse_map, vUV).rgb;

    // specular
    vec3 view_direction = normalize(uViewPosition - vWorldPos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_scalar = pow(max(dot(view_direction, reflect_direction), 0.0), uMaterial.shininess);
    vec3 specular = uDirectionalLight.specular * specular_scalar * texture(uMaterial.specular_map, vUV).rgb;

    return vec3 ( ambient + diffuse + specular ) ;
}

vec3 SpotLight()
{
    // ambient
    vec3 ambient = uSpotLight.ambient * texture(uMaterial.diffuse_map, vUV).rgb;

    // diffuse
	vec3 normal = normalize(vNormal);
    vec3 light_direction = normalize(uSpotLight.position - vWorldPos);
    float diffuse_scalar = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = uSpotLight.diffuse * diffuse_scalar * texture(uMaterial.diffuse_map, vUV).rgb;

    // specular
    vec3 view_direction = normalize(uSpotLight.viewPos - vWorldPos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_scalar = pow(max(dot(view_direction, reflect_direction), 0.0), uMaterial.shininess);
    vec3 specular = uSpotLight.specular * specular_scalar * texture(uMaterial.specular_map, vUV).rgb;

    // spotlight (soft edges)
    float theta = dot(light_direction, normalize(-uSpotLight.direction)); 
    float epsilon = (uSpotLight.cutOff - uSpotLight.outerCutOff);
    float intensity = clamp ((theta - uSpotLight.outerCutOff)/epsilon, 0.0 ,1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance    = length(uSpotLight.position - vWorldPos);
    float attenuation = 1.0 / (uSpotLight.constant + uSpotLight.linear * distance + uSpotLight.quadratic * (distance * distance));  

    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return vec3 ( ambient + diffuse + specular ) ;
}

void main()
{
    // vec3 light_value{0,0,0};
    // for (int i = 0; i < number_of_point_light; ++i)
    // {
    //     light_value += uPointLight[i];
    // }
    // // concatenate all directional lights
    // for (int i = 0; i < number_of_directional_light; ++i)
    // {
    //     light_value += uDirectionalLight[i];
    // }
    
    // concatenate all point lights
    // fFragColor = vec4(PointLight() + DirectLight() + SpotLight() , 1.0);
    fFragColor = vec4(PointLight() + DirectLight()  , 1.0);
}
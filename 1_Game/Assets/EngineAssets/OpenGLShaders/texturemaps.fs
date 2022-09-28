#version 430 core

out vec4 fFragColor;

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
    vec3 position; // for directional shadows
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

uniform sLight              uPointLight[5];
uniform int                 uNumPointLight;

uniform sDirectionalLight   uDirectionalLight[5];
uniform int                 uNumDirectionalLight;

uniform sSpotLight          uSpotLight;

//skybox
uniform samplerCube uSkybox;

// shadowmap
uniform sampler2D uShadowMap;

struct sFogParameters
{
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	
	int equation;
	bool isEnabled;
};

// fog
uniform sFogParameters uFogParams;

float light_max_distance = 100.0f;
float gamma = 2.2;

float getFogFactor(sFogParameters params, float fogCoordinate)
{
	float result = 0.0;

    // linear interpolation
	if(params.equation == 0)
	{
		float fogLength = params.linearEnd - params.linearStart;
		result = (params.linearEnd - fogCoordinate) / fogLength;
	}
    //  won't make fog to appear linearly, but rather we use an exponential function
	else if(params.equation == 1) {
		result = exp(-params.density * fogCoordinate);
	}
    // similar to previous, but this time we'll use the power of density
	else if(params.equation == 2) {
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPosition)
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
    vec3 light_dir = normalize(lightPosition - vWorldPos);
    float bias = max(0.05 * (1.0 - dot(vNormal, light_dir)), 0.003);

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

vec3 PointLight(int i)
{
    vec3 normal = normalize(vNormal);
    vec3 light_to_frag = uPointLight[i].position - vWorldPos;
    float light_distance = length(light_to_frag);
    float attenuation = 1.0 / light_distance;



    vec3 light_direction = normalize(light_to_frag);
    vec3 view_direction = normalize(uViewPosition - vWorldPos);
    vec3 reflect_direction = reflect(-light_direction, normal);

    // ambient
    vec4 ambient = vec4((uPointLight[i].colour * uPointLight[i].ambient), 1.0) * texture(uMaterial.diffuse_map, vUV);

    // diffuse
    float diffuse_scalar = max(dot(normal, light_direction), 0.0);
    diffuse_scalar  *= attenuation;

    vec4 diffuse = vec4((uPointLight[i].colour * uPointLight[i].diffuse), 1.0) * (diffuse_scalar * vec4(pow(texture(uMaterial.diffuse_map, vUV).rgb,vec3(gamma)),1.0));

    // cubemap reflection
    vec3 I = normalize (vWorldPos - uViewPosition) ; //--
    vec3 R = reflect (I ,normalize(vNormal)) ; //--
    R.z = R.z * -1.0f; //--
    vec4 cubemap_colour =  vec4(texture(uSkybox, R).rgb, 1.0);

    // specular
    float specular_scalar = pow(max(dot(view_direction, reflect_direction), 0.0), uMaterial.shininess);
    specular_scalar *= attenuation;
    vec4 specular = cubemap_colour * (specular_scalar * texture(uMaterial.specular_map, vUV));



    // calculate shadow
    // float shadow = ShadowCalculation(vFragPosLightSpace);
    return vec3((ambient + ((light_max_distance - light_distance) / light_max_distance) * (diffuse + specular)) * texture(uMaterial.diffuse_map, vUV));
    // return vec3((ambient + (1.0 - shadow) * (diffuse + specular)) * texture(uMaterial.diffuse_map, vUV));
}


vec3 DirectLight(int i, float shadow)
{
    // ambient
    vec3 ambient = uDirectionalLight[i].ambient * texture(uMaterial.diffuse_map, vUV).rgb;

    // diffuse
	vec3 normal = normalize(vNormal);
    vec3 light_direction = normalize(-uDirectionalLight[i].direction);
    float diffuse_scalar = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = uDirectionalLight[i].diffuse * diffuse_scalar * pow(texture(uMaterial.diffuse_map, vUV).rgb,vec3(gamma));

    // specular
    vec3 view_direction = normalize(uViewPosition - vWorldPos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float specular_scalar = pow(max(dot(view_direction, reflect_direction), 0.0), uMaterial.shininess);
    vec3 specular = uDirectionalLight[i].specular * specular_scalar * texture(uMaterial.specular_map, vUV).rgb;

    return vec3 ( ambient + (1.0 - shadow) * (diffuse + specular) ) ;
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
    // calculate shadow
    vec3 light_position = normalize(vec3(0,0,0));
    if (uNumDirectionalLight > 0)
    {
        light_position = uDirectionalLight[0].position;
    }
    float shadow = ShadowCalculation(vFragPosLightSpace, light_position);

    // calculate point lights
    vec3 point_lights_value = vec3(0,0,0);
    for (int i = 0; i < uNumPointLight; ++i)
    {
        point_lights_value += PointLight(i);
    }

    // calculate directional lights
    vec3 directional_lights_value = vec3(0,0,0);
    for (int i = 0; i < 1; ++i)
    {
        directional_lights_value += DirectLight(i,shadow);
    }

    fFragColor = vec4((point_lights_value + directional_lights_value) , 1.0);

    //apply fog calculation only if fog is enabled
    // if(uFogParams.isEnabled)
    // {
    //       float fogCoordinate = abs(vEyeSpacePosition.z / vEyeSpacePosition.w);
    //       fFragColor = mix(fFragColor, vec4(uFogParams.color, 1.0), getFogFactor(uFogParams, fogCoordinate));
    // }
}
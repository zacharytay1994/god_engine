#version 430 core

out vec4 fFragColor;

in vec3 vNormal;
in vec3 vWorldPos;

uniform vec3 uViewPosition;

// material properties
struct Material 
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
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

vec3 LightValue()
{
    vec3 normal = normalize(vNormal);
    vec3 light_direction = normalize(uLight.position - vWorldPos);
    vec3 view_direction = normalize(uViewPosition - vWorldPos);
    vec3 reflect_direction = reflect(-light_direction, normal);

    // ambient
    vec4 ambient = vec4((uLight.colour * uLight.ambient), 1.0) * uMaterial.ambient;

    // diffuse
    float diffuse_scalar = max(dot(normal, light_direction), 0.0);
    vec4 diffuse = vec4((uLight.colour * uLight.diffuse), 1.0) * (diffuse_scalar * uMaterial.diffuse);

    // specular
    float specular_scalar = pow(max(dot(view_direction, reflect_direction), 0.0), uMaterial.shininess);
    vec4 specular = vec4(uLight.specular, 1.0) * (specular_scalar * uMaterial.specular);

    return ambient + diffuse + specular;
}

void main()
{
    fFragColor =  vec4(LightValue(), 1.0);
}
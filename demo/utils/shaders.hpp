#pragma once

static const char *vertexShaderSource = R"(
#version 330 core

// ======== INPUT ========
in vec3 avPosition;
in vec3 avNormal;

uniform mat4 uView;
uniform mat4 uProjection;


// ======== OUTPUT ========
out VS_OUT {
    vec3 position;
    vec3 normal;
} vs_out;


// ======== MAIN ========
void main()
{
    vs_out.position = avPosition;
    vs_out.normal = avNormal;

    gl_Position = uProjection * uView * vec4(avPosition, 1.0);
}
)";


static const char *fragmentShaderSource = R"(
#version 330 core

// ======== TYPES ========
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


// ======== INPUT ========
in VS_OUT {
    vec3 position;
    vec3 normal;
} fs_in;

uniform vec3 uCameraPosition;
uniform Material uMaterial;
uniform Light uLight;


// ======== OUTPUT ========
out vec4 fs_out_color;


// ======== MAIN ========
void main()
{
    vec3 viewDirection = normalize(uCameraPosition - fs_in.position);
    vec3 lightDirection = normalize(uLight.position - fs_in.position);

    float diffuseImpact = max(dot(fs_in.normal, lightDirection), 0.0);

    vec3 reflectDirection = reflect(-lightDirection, fs_in.normal);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);

    vec3 ambient = uLight.ambient * uMaterial.diffuse;
    vec3 diffuse = uLight.diffuse * diffuseImpact * uMaterial.diffuse;
    vec3 specular = uLight.specular * specularImpact * uMaterial.specular;

    fs_out_color = vec4(ambient + diffuse + specular, 1.0);
}
)";
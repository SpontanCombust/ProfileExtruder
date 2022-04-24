#pragma once

static const char *vertexShaderSource = R"(
#version 330 core

// ======== INPUT ========
in vec3 avPosition;
in vec3 avNormal;
in vec2 avUV;

uniform mat4 uView;
uniform mat4 uProjection;


// ======== OUTPUT ========
out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;


// ======== MAIN ========
void main()
{
    vs_out.position = avPosition;
    vs_out.normal = avNormal;
    vs_out.uv = avUV;

    gl_Position = uProjection * uView * vec4(avPosition, 1.0);
}
)";


static const char *fragmentShaderSource = R"(
#version 330 core

// ======== TYPES ========
struct Material {
    sampled2D diffuse;
    float specular;
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
    vec2 uv;
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

    vec3 texel = texture(uMaterial.diffuse, fs_in.uv).rgb;
    vec3 ambient = light.ambient * texel;
    vec3 diffuse = light.diffuse * diffuseImpact * texel;
    vec3 specular = light.specular * specularImpact * texel;

    fs_out_color = vec4(ambient + diffuse + specular, 1.0);
}
)";
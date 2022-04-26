#version 330 core

// ======== INPUT ========
in vec3 avPosition;
in vec3 avNormal;

uniform vec3 uTranslation = vec3(0.0, 0.0, 0.0);
uniform float uScale = 1.0;
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
    vec3 vertex = avPosition * uScale + uTranslation;

    vs_out.position = vertex;
    vs_out.normal = avNormal;

    gl_Position = uProjection * uView * vec4(vertex, 1.0);
}
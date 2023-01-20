#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    float ratio = 1.0 / 1.52;
    vec3 I = normalize(position - cameraPos);
    vec3 R = refract(I, normalize(normal), ratio);
    fragColor = vec4(texture(skybox, R).rgb, 1.0);
}
#version 330 core

in vec3 normal;
in vec2 texCoord;
in vec3 position;

out vec4 fragColor;

uniform vec3 direction;
uniform vec3 color;

void main() {
    // abient
	vec3 ambient =  color * 0.3f;
 
    // diffuse
    vec3 lightDir = normalize(direction);
    
    vec3 result = ambient;

    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    result += diffuse;

	fragColor = vec4(result, 1.0);
    // fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
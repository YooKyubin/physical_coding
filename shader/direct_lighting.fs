#version 330 core

in vec3 normal;
in vec2 texCoord;
in vec3 position;

out vec4 fragColor;

uniform vec3 direction;
uniform vec3 color;
uniform vec3 viewPos;

void main() {
    // abient
	vec3 ambient =  color * 0.3;
 
    // diffuse
    vec3 lightDir = normalize(direction);
    
    vec3 result = ambient;

    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    //specular
    vec3 specColor = vec3(1.0, 1.0, 1.0);
    float spec = 0.0;
    float shininess = 20;

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 specular = spec * specColor;

    result += diffuse + specular;

	fragColor = vec4(result, 1.0);
    // fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
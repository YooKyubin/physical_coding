#version 330 core

in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
    fragColor = texture(tex, texCoord) * 0.2 + texture(tex2, texCoord) * 0.2
     + vec4(1.0f, 0.01f, 0.31f, 1.0f)* 0.6;
}
#version 330 core

in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    fragColor = texture(tex, texCoord);
}

// 여러장의 텍스처를 하나의 쉐이더에서 불러서 사용할 수 있다는걸 보여줬었음
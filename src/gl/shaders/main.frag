#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D u_diffuse0;

void main() {
  FragColor = vec4(texture(u_diffuse0, texCoord).rgb, 1.f);
}


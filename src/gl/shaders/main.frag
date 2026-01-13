#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D u_terrainTex;

void main() {
  FragColor = vec4(texture(u_terrainTex, texCoord).rgb, 1.f);
}


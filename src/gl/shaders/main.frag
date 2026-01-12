#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D u_noiseTex;

void main() {
  FragColor = vec4(texture(u_noiseTex, texCoord).rgb, 1.f);
}


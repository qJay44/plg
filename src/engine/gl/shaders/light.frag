#version 460 core

in vec3 color;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 u_pos;
uniform vec3 u_camPos;

void main() {
  float dist = length(texCoord - 0.5f);
  float alpha = 1.f - smoothstep(0.f, 0.5f, dist);

  FragColor = vec4(color, alpha);
}


#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTex;
layout(location = 3) in vec3 inNormal;

out vec2 texCoord;

uniform mat4 u_cam;
uniform mat4 u_model;

void main() {
  texCoord = inTex;
  gl_Position = u_cam * u_model * vec4(inPos, 1.0f);
}


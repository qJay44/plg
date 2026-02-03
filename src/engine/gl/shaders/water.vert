#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;

out vec4 worldPos;
out vec4 clipSpace;
out vec2 texCoord;

uniform mat4 u_camPV;
uniform mat4 u_model;
uniform vec2 u_offset;
uniform vec2 u_chunks;
uniform float u_tiling;

void main() {
  worldPos = u_model * vec4(inPos, 1.f);
  clipSpace = u_camPV * worldPos;
  texCoord = (inTex + u_offset) * u_tiling;
  gl_Position = clipSpace;
}


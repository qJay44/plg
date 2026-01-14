#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTex;
layout(location = 3) in vec3 inNormal;

out vec2 uvs;

void main() {
  uvs = inTex;
  gl_Position = vec4(inPos, 1.f);
}


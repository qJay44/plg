#version 460 core

layout (vertices = 4) out;

in vec2 uvs[];
out vec2 uvsCoord[];

uniform int u_div;

void main() {
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  uvsCoord[gl_InvocationID] = uvs[gl_InvocationID];

  gl_TessLevelOuter[0] = u_div;
  gl_TessLevelOuter[1] = u_div;
  gl_TessLevelOuter[2] = u_div;
  gl_TessLevelOuter[3] = u_div;

  gl_TessLevelInner[0] = u_div;
  gl_TessLevelInner[1] = u_div;
}


#version 460 core

layout (quads, equal_spacing, cw) in;

in vec2 uvsCoord[];

out vec2 texCoord;
out vec2 chunkTexCoord;

uniform sampler2D u_terrainTex;
uniform vec2 u_chunks;
uniform vec2 u_chunkOffset;
uniform mat4 u_cam;
uniform float u_heightMultiplier;

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  vec2 uv0 = uvsCoord[0];
  vec2 uv1 = uvsCoord[1];
  vec2 uv2 = uvsCoord[2];
  vec2 uv3 = uvsCoord[3];

  vec2 leftUV  = uv0 + v * (uv3 - uv0);
  vec2 rightUV = uv1 + v * (uv2 - uv1);
  texCoord = leftUV + u * (rightUV - leftUV);
  chunkTexCoord = texCoord / u_chunks + u_chunkOffset;

  vec4 pos0 = gl_in[0].gl_Position;
  vec4 pos1 = gl_in[1].gl_Position;
  vec4 pos2 = gl_in[2].gl_Position;
  vec4 pos3 = gl_in[3].gl_Position;

  vec4 leftPos  = pos0 + v * (pos3 - pos0);
  vec4 rightPos = pos1 + v * (pos2 - pos1);
  vec4 pos = leftPos + u * (rightPos - leftPos);

  float height = texture(u_terrainTex, chunkTexCoord).a;
  height = pow(2, (max(height, 0.4f) - 0.4f) * u_heightMultiplier) - 1.f;
  pos.y += height;

  gl_Position = u_cam * pos;
}


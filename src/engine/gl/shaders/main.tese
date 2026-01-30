#version 460 core

layout (quads, equal_spacing, cw) in;

in vec2 uvsCoord[];

out DATA {
  vec4 vertPos;
  vec2 texCoord;
  vec2 chunkTexCoord;
  vec3 chunkNormal;
} dataOut;

uniform sampler2D u_noiseTex;
uniform sampler2D u_falloffTex;
uniform vec2 u_chunks;
uniform vec2 u_chunkOffset;
uniform mat4 u_camPV;
uniform float u_heightMultiplier;
uniform float u_maskFalloffmap;

float getHeight(vec2 uv) {
  float h = texture(u_noiseTex, uv).r;
  float f = texture(u_falloffTex, uv).r * u_maskFalloffmap;

  h = max(h - f, 0.f);

  return exp(h * u_heightMultiplier);
}

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;
  vec2 texelSize = 1.f / textureSize(u_noiseTex, 0);

  vec2 uv0 = mix(uvsCoord[0], uvsCoord[1], u);
  vec2 uv1 = mix(uvsCoord[3], uvsCoord[2], u);
  vec2 texCoord = mix(uv0, uv1, v);
  vec2 chunkTexCoord = texCoord / u_chunks + u_chunkOffset;

  vec4 p0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, u);
  vec4 p1 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, u);
  vec4 vertPos = mix(p0, p1, v);

  float h = getHeight(chunkTexCoord);
  float hR = getHeight(chunkTexCoord + vec2(texelSize.x, 0.f));
  float hT = getHeight(chunkTexCoord + vec2(0.f, texelSize.y));

  vertPos.y += h;

  vec2 gridStep = vec2(1.f); // terrainWidth / textureSize(u_noiseTex, 0)
  vec3 tangent = vec3(gridStep.x, hR - h, 0.f);
  vec3 bitangent = vec3(0.f, hT - h, gridStep.y);
  vec3 chunkNormal = normalize(cross(bitangent, tangent));

  dataOut.vertPos       = vertPos;
  dataOut.texCoord      = texCoord;
  dataOut.chunkTexCoord = chunkTexCoord;
  dataOut.chunkNormal   = chunkNormal;

  gl_Position = u_camPV * vertPos;
}


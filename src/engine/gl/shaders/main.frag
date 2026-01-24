#version 460 core

in DATA {
  vec4 vertPos;
  vec2 texCoord;
  vec2 chunkTexCoord;
  vec3 chunkNormal;
} dataIn;

out vec4 FragColor;

uniform sampler2D u_terrainTex;
uniform vec3 u_chunkDebugColor;
uniform float u_maskDebugColors;
uniform float u_maskNormalmap;

void main() {
  vec3 col = texture(u_terrainTex, dataIn.chunkTexCoord).rgb;
  col = col * (1.f - u_maskDebugColors) + u_chunkDebugColor * u_maskDebugColors;
  col = col * (1.f - u_maskNormalmap) + (dataIn.chunkNormal * 0.5f + 0.5f) * u_maskNormalmap;

  FragColor = vec4(col, 1.f);
}


#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D u_terrainTex;
uniform vec3 u_chunkDebugColor;
uniform bool u_useDebugColors;

void main() {
  float dbgColMask = float(u_useDebugColors);
  vec3 tex = texture(u_terrainTex, texCoord).rgb;
  vec3 col = tex * (1.f - dbgColMask) + u_chunkDebugColor * dbgColMask;
  FragColor = vec4(col, 1.f);
}


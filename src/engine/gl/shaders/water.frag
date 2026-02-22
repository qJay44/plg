#version 460 core

out vec4 FragColor;

in vec4 worldPos;
in vec4 clipSpace;
in vec2 texCoord;
in vec3 toLightDir;

layout(binding = 0) uniform sampler2D u_reflectionTex;
layout(binding = 1) uniform sampler2D u_refractionTex;
layout(binding = 2) uniform sampler2D u_dudvMapTex;
layout(binding = 3) uniform sampler2D u_reflectionDepthTex;
layout(binding = 4) uniform sampler2D u_refractionDepthTex;
layout(binding = 5) uniform sampler2D u_normalMapTex;

uniform vec3 u_camPos;
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;
uniform float u_camNear;
uniform float u_camFar;
uniform float u_time;
uniform float u_waveStrength;
uniform float u_waveFreq;
uniform float u_specularDamper;
uniform float u_specularStrength;

vec3 specularLight(vec3 viewDir, vec3 toLightDir, vec3 normal) {
  vec3 reflDir = reflect(toLightDir, normal); // Reflected light from the world point
  float specular = max(dot(reflDir, viewDir), 0.f);
  specular = pow(specular, u_specularDamper);

  return u_lightColor * specular * u_specularStrength;
}

float linearizeDepth(float depth) {
  float z = depth * 2.f - 1.f;
  return (2.f * u_camNear * u_camFar) / (u_camFar + u_camNear - z * (u_camFar - u_camNear));
}

void main() {
  vec3 viewDir = normalize(u_camPos - worldPos.xyz);
  vec3 toLightDir = normalize(worldPos.xyz - u_lightPos); // From light the to world point
  vec2 ndc = clipSpace.xy / clipSpace.w * 0.5f + 0.5f;
  vec2 reflTexCoord = vec2(ndc.x, 1.f - ndc.y);
  vec2 refrTexCoord = ndc;

  float floorDist = linearizeDepth(texture(u_refractionDepthTex, refrTexCoord).r);
  float waterDist = linearizeDepth(gl_FragCoord.z);
  float waterDepth = floorDist - waterDist;

  vec2 distortedTexCoords = texture(u_dudvMapTex, vec2(texCoord.x + u_time * u_waveFreq, texCoord.y)).rg * 0.1f;
  distortedTexCoords = texCoord + vec2(distortedTexCoords.x, distortedTexCoords.y + u_time * u_waveFreq);
  vec2 totalDistortion = (texture(u_dudvMapTex, distortedTexCoords).rg * 2.f - 1.f) * u_waveStrength;
  totalDistortion *= clamp(waterDepth * 0.05f, 0.f, 1.f);

  reflTexCoord += totalDistortion;
  refrTexCoord += totalDistortion;

  reflTexCoord = clamp(reflTexCoord, 0.001f, 0.999f);
  refrTexCoord = clamp(refrTexCoord, 0.001f, 0.999f);

  vec3 normalMapCol = texture(u_normalMapTex, distortedTexCoords).rgb;
  vec3 waterNormal = normalize(vec3(
    normalMapCol.r * 2.f - 1.f,
    normalMapCol.b * 3.f, // Make them more upper
    normalMapCol.g * 2.f - 1.f
  ));

  vec3 reflCol = texture(u_reflectionTex, reflTexCoord).rgb;
  vec3 refrCol = texture(u_refractionTex, refrTexCoord).rgb;

  float fresnel = dot(viewDir, waterNormal);
  vec3 col = mix(reflCol, refrCol, pow(fresnel, 0.5f));
  col += specularLight(viewDir, toLightDir, waterNormal) * clamp(waterDepth * 0.2f, 0.f, 1.f);

  float alpha = clamp(waterDepth * 0.1f, 0.f, 1.f);

  FragColor = vec4(col, alpha);
}


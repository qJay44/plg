#version 460 core

out vec4 FragColor;

uniform vec3 u_camPos;
uniform vec3 u_lightPos;
uniform vec3 u_skyHorizonColor;
uniform vec3 u_skyZenithColor;
uniform vec3 u_groundColor;
uniform mat4 u_camInv;

void main() {
  vec2 uv = vec2(gl_FragCoord.xy) / vec2(1200, 720);
  vec2 ndc = uv * 2.f - 1.f;
  vec4 clipPos = vec4(ndc, -1.f, 1.f);
  vec4 worldPos = u_camInv * clipPos;
  worldPos /= worldPos.w;

  vec3 ro = u_camPos;
  vec3 rd = normalize(worldPos.xyz - u_camPos);

  vec3 lightDir = normalize(ro - u_lightPos);
  float skyGradientT = pow(smoothstep(0.f, 0.4f, rd.y), 0.35f);
  vec3 skyGradient = mix(u_skyHorizonColor, u_skyZenithColor, skyGradientT);
  float groundToSkyT = smoothstep(-0.01f, 0.f, rd.y);
  vec3 col = mix(u_groundColor, skyGradient, groundToSkyT);

  FragColor = vec4(col, 1.f);
}


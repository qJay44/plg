#include "noise.hpp"
#include "gl/texture/image2D.hpp"

#include <cmath>

namespace noise {

float dot(vec2 a, vec2 b) {
  return a.x * b.x + a.y * b.y;
}

float fade(float t) {
  return t*t*t*(t*(t*6.f - 15.f) + 10.f);
}

vec2 grad(vec2 p) {
  static const image2D permutationImg("res/rgba-noise-medium.png");
  p.x /= permutationImg.width;
  p.y /= permutationImg.height;

  ivec2 pp(p);
  pp.x %= permutationImg.width;
  pp.y %= permutationImg.height;

  byte* pix = ((byte*)permutationImg.pixels) + (pp.y + pp.x * permutationImg.height) * 4;
  float x = *(pix + 0);
  float y = *(pix + 1);

  x = x / 255.f * 2.f - 1.f;
  y = y / 255.f * 2.f - 1.f;

  return {x, y};
}

float perlin(vec2 p) {
  vec2 p0{std::floor(p.x), std::floor(p.x)};
  vec2 p1 = p0 + vec2{1.f, 0.f};
  vec2 p2 = p0 + vec2{0.f, 1.f};
  vec2 p3 = p0 + vec2{1.f, 1.f};

  vec2 g0 = grad(p0);
  vec2 g1 = grad(p1);
  vec2 g2 = grad(p2);
  vec2 g3 = grad(p3);

  float t0 = p.x - p0.x;
  float fade0 = fade(t0);

  float t1 = p.y - p0.y;
  float fade1 = fade(t1);

  float p0p1 = (1.f - fade0) * dot(g0, (p - p0)) + fade0 * dot(g1, (p - p1));
  float p2p3 = (1.f - fade0) * dot(g2, (p - p2)) + fade0 * dot(g3, (p - p3));

  return (1.f - fade1) * p0p1 + fade1 * p2p3;
}

} // namespace noise


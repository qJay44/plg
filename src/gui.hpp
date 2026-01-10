#pragma once

#include "gl/texture/Texture.hpp"

struct gui {
  static std::string performanceInfo;
  static Texture* noiseTex;

  static void toggle();
  static void draw();
};


#pragma once

#include "Terrain.hpp"

struct gui {
  static u16 fps;
  static Terrain* terrainPtr;

  static void toggle();
  static void draw();
};


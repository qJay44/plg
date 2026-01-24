#pragma once

#include "Character.hpp"
#include "terrain/Terrain.hpp"

struct gui {
  static u16 fps;
  static Terrain* terrainPtr;
  static Character* characterPtr;

  static void toggle();
  static void draw();
};


#pragma once

#include "MapGenerator.hpp"

struct gui {
  static u16 fps;
  static MapGenerator* mg;

  static void toggle();
  static void draw();
};


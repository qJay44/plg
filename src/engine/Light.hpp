#pragma once

#include "Moveable.hpp"
#include "gl/mesh/Mesh.hpp"
#include "gl/mesh/meshes.hpp"

class Light : public Mesh, public Moveable {
public:
  Light(vec3 pos, float meshScale = 1.f) : Mesh(meshes::cube()), Moveable(pos, 0.f, 0.f) {
    translate(pos);
    scale(meshScale);
  }

  void update() {
    setTrans(position);
  }

private:
  friend struct gui;
};


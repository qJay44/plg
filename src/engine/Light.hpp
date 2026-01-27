#pragma once

#include "Moveable.hpp"
#include "gl/Shader.hpp"
#include "gl/mesh/Mesh.hpp"
#include "gl/mesh/meshes.hpp"

class Light : public Mesh, public Moveable {
public:
  Light(vec3 pos, float meshScale = 1.f) : Mesh(meshes::plane()), Moveable(pos, 0.f, 0.f) {
    translate(pos);
    scale(meshScale);
  }

  void update() {
    setTrans(position);
  }

  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const {
    shader.setUniform3f("u_pos", position);
    shader.setUniform1f("u_radius", radius);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh::draw(camera, shader);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }

private:
  friend struct gui;

  float radius = 15.f;
};


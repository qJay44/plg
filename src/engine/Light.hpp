#pragma once

#include "Moveable.hpp"
#include "gl/Shader.hpp"
#include "gl/mesh/Mesh.hpp"
#include "gl/mesh/meshes.hpp"
#include "../global.hpp"

class Light : public Mesh, public Moveable {
public:
  Light(vec3 pos, float radius, vec3 color = vec3(1.f))
    : Mesh(meshes::plane(2, GL_TRIANGLES, global::forward)), Moveable(pos, 0.f, 0.f), radius(radius), color(color) {
    translate(pos);
    scale(radius);
  }

  const vec3& getColor() const {
    return color;
  }

  void update() {
    setTrans(position);
  }

  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const {
    shader.setUniform1f("u_radius", radius);
    shader.setUniform3f("u_pos", position);
    shader.setUniform3f("u_col", color);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh::draw(camera, shader);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }

private:
  friend struct gui;

  float radius;
  vec3 color;
};


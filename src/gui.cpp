#include "gui.hpp"

#include "MapGenerator.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

using namespace ImGui;
using global::camera;

static bool collapsed = true;

u16 gui::fps = 1;
MapGenerator* gui::mg = nullptr;

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  ImGui::Text("FPS: %d / %f.5 ms", fps, global::dt);

  // ================== Camera ========================= //

  if (!camera) error("[gui] The camera is not linked to gui");

  if (TreeNode("Camera")) {
    SliderFloat("Near##2",  &camera->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2",   &camera->farPlane,  10.f , 1000.f);
    SliderFloat("Speed##2", &camera->speed,     1.f  , 50.f);
    SliderFloat("FOV##2",   &camera->fov,       45.f , 179.f);

    Spacing();
    TextColored({0.f, 1.f, 1.f, 1.f}, "Position");
    Text("%.5f, %.5f, %.5f", camera->position.x, camera->position.y, camera->position.z);

    Spacing();
    TextColored(vec4(camera->orientation * 0.5f + 0.5f, 1.f), "Orientation");
    Text("%.5f, %.5f, %.5f", camera->orientation.x, camera->orientation.y, camera->orientation.z);

    TreePop();
  }

  // ================== Map Generator ================== //

  if (!mg) error("[gui] The MapGenerator is not linked to gui");

  if (TreeNode("Noise texture")) {
    static float imgScale = 0.5f;
    bool upd = false;

    upd |= SliderInt("Width", &mg->size.x, 1, 512);
    upd |= SliderInt("Height", &mg->size.y, 1, 512);
    upd |= SliderFloat("Scale", &mg->scale, 0.01f, 100.f);
    upd |= SliderFloat("Persistance", &mg->persistance, 0.01f, 1.f);
    upd |= SliderFloat("Lacunarity", &mg->lacunarity, 1.f, 100.f);
    upd |= SliderInt("Octaves", &mg->octaves, 1, 10);
    upd |= DragInt("Seed", &mg->seed);
    upd |= DragFloat2("Offset", glm::value_ptr(mg->offset), 0.25f);

    if (upd)
      mg->gen();

    Spacing();
    SliderFloat("Image scale", &imgScale, 0.01f, 1.f);
    Image(mg->tex.getId(), vec2(mg->size) * imgScale);

    TreePop();
  }

  // ================== Other ========================== //

  if (TreeNode("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);
    Checkbox("Wireframe mod", &global::drawWireframe);

    TreePop();
  }

  End();
}


#include "gui.hpp"

#include "MapGenerator.hpp"
#include "imgui.h"

using namespace ImGui;
using global::camera;

static bool collapsed = true;

std::string gui::performanceInfo = "Debug";
Texture* gui::noiseTex = nullptr;

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  ImGui::Text("%s", performanceInfo.c_str());

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

  if (!noiseTex) error("[gui] The noise texture is not linked to gui");

  if (TreeNode("Noise texture")) {
    static ivec2 size{50, 20};
    static float scale = 0.3f;
    bool upd = false;

    upd |= SliderInt("Width", &size.x, 1, 512);
    upd |= SliderInt("Height", &size.y, 1, 512);
    upd |= SliderFloat("Scale", &scale, 0.01f, 100.f);

    if (upd)
      *noiseTex = MapGenerator::gen(size, scale);

    TreePop();
  }

  // ================== Other ========================== //

  if (TreeNode("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);

    TreePop();
  }

  End();
}


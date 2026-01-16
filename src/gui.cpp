#include "gui.hpp"

#include "MapGenerator.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include <string>

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

  if (CollapsingHeader("Camera")) {
    if (!camera) error("[gui] The camera is not linked to gui");

    SliderFloat("Near##2",  &camera->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2",   &camera->farPlane,  10.f , 1000.f);
    SliderFloat("Speed##2", &camera->speed,     1.f  , 1000.f);
    SliderFloat("FOV##2",   &camera->fov,       45.f , 179.f);

    Spacing();
    TextColored({0.f, 1.f, 1.f, 1.f}, "Position");
    Text("%.5f, %.5f, %.5f", camera->position.x, camera->position.y, camera->position.z);

    Spacing();
    TextColored(vec4(camera->orientation * 0.5f + 0.5f, 1.f), "Orientation");
    Text("%.5f, %.5f, %.5f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
  }

  // ================== Noise texture ================== //

  if (!mg) error("[gui] The MapGenerator is not linked to gui");

  bool reGenTex = false;

  if (CollapsingHeader("Noise texture")) {
    static float imgScale = 0.5f;
    static bool sq = true;

    reGenTex |= Checkbox("Width = Height", &sq);

    if (SliderInt("Width", &mg->size.x, 1, 4096)) {
      if (sq) mg->size.y = mg->size.x;
      reGenTex = true;
    }

    if (SliderInt("Height", &mg->size.y, 1, 4096)) {
      if (sq) mg->size.x = mg->size.y;
      reGenTex = true;
    }

    reGenTex |= SliderFloat("Scale", &mg->scale, 0.01f, 1000.f);
    reGenTex |= SliderFloat("Persistance", &mg->persistance, 0.01f, 1.f);
    reGenTex |= SliderFloat("Lacunarity", &mg->lacunarity, 1.f, 100.f);
    reGenTex |= SliderInt("Octaves", &mg->octaves, 1, 10);
    reGenTex |= DragInt("Seed", &mg->seed, 0.1f);
    reGenTex |= DragFloat2("Offset", glm::value_ptr(mg->offset), 0.1f);

    Spacing();
    SliderFloat("Image scale", &imgScale, 0.01f, 1.f);
    Image(mg->noiseTex.getId(), vec2(mg->size) * imgScale);
  }

  // ================== Terrain ======================== //

  if (CollapsingHeader("Terrain")) {
    SliderFloat("TESC divisions", &mg->tescDiv, 1.f, 1024.f);
    SliderFloat("Height multiplier", &mg->heightMultiplier, 0.1f, 20.f);

    if (SliderFloat("Plane scale", &mg->planeScale, 0.1f, 1000.f))
      mg->plane.setScale({mg->planeScale, 1.f, mg->planeScale});

    if (TreeNode("Regions")) {
      for (size_t i = 0; i < mg->regions.size(); i++) {
        std::string name = mg->regions[i].uniformFmt;
        name.pop_back();
        name.pop_back();
        name = name.substr(2);

        reGenTex |= SliderFloat((name + " height").c_str(), &mg->regions[i].height, 0.f, 1.f);
        reGenTex |= ColorEdit3((name + " color").c_str(), glm::value_ptr(mg->regions[i].color));
      }

      TreePop();
    }
  }

  if (reGenTex)
    mg->gen();

  // ================== Other ========================== //

  if (CollapsingHeader("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);
    Checkbox("Wireframe mod", &global::drawWireframe);
  }

  End();
}


#include "gui.hpp"

#include <string>

#include "glm/gtc/type_ptr.hpp"
#include "../global.hpp"

#include "imgui.h"

using namespace ImGui;

static const GLint swizzle[4] = {GL_RED, GL_RED, GL_RED, GL_ONE};
static bool collapsed = true;

u16 gui::fps = 1;
Terrain* gui::terrainPtr = nullptr;
Character* gui::characterPtr = nullptr;
Light* gui::lightPtr = nullptr;

void gui::toggle() { collapsed = !collapsed; }

void gui::draw() {
  static RunOnce a([]() {
    SetNextWindowPos({0, 0});
  });
  SetNextWindowCollapsed(collapsed);

  Begin("Settings");

  ImGui::Text("FPS: %d / %f.5 ms", fps, global::dt);

  // ================== Camera ========================= //

  Camera* camera = characterPtr->cam;

  if (CollapsingHeader("Camera")) {
    SliderFloat("Near", &camera->nearPlane, 0.01f, 1.f);
    SliderFloat("Far", &camera->farPlane, 10.f, 1000.f);
    SliderFloat("FOV", &camera->fov, 45.f, 179.f);

    Spacing();
    TextColored({0.f, 1.f, 1.f, 1.f}, "Position");
    Text("%.5f, %.5f, %.5f", camera->position.x, camera->position.y, camera->position.z);

    Spacing();
    TextColored(vec4(camera->orientation * 0.5f + 0.5f, 1.f), "Orientation");
    Text("%.5f, %.5f, %.5f", camera->orientation.x, camera->orientation.y, camera->orientation.z);
  }

  // ================== Noise texture ================== //

  if (!terrainPtr) error("[gui] The terrain is not linked to gui");

  MapGenerator& mg = terrainPtr->sharedMapGen;
  bool reGenTex = false;

  if (CollapsingHeader("Noise texture")) {
    static float imgScale = 0.5f;
    static bool sq = true;

    reGenTex |= Checkbox("Width = Height", &sq);
    SetItemTooltip("Better to keep it on");

    if (SliderInt("Width", &mg.size.x, 1, 4096)) {
      if (sq) mg.size.y = mg.size.x;
      reGenTex = true;
    }

    if (SliderInt("Height", &mg.size.y, 1, 4096)) {
      if (sq) mg.size.x = mg.size.y;
      reGenTex = true;
    }

    reGenTex |= SliderFloat("Scale", &mg.scale, 0.01f, 1000.f);
    reGenTex |= SliderFloat("Persistance", &mg.persistance, 0.01f, 1.f);
    reGenTex |= SliderFloat("Lacunarity", &mg.lacunarity, 1.f, 100.f);
    reGenTex |= SliderFloat("Noise max reduce", &mg.noiseMaxReducer, -5.f, 5.f);
    reGenTex |= SliderInt("Octaves", &mg.octaves, 1, 10);
    reGenTex |= DragInt("Seed", &mg.seed, 0.1f);
    reGenTex |= DragFloat2("Offset", glm::value_ptr(mg.offset), 0.1f);

    Spacing();
    SliderFloat("Image scale", &imgScale, 0.01f, 1.f);

    mg.noiseTex.bind();
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    Image(mg.noiseTex.getId(), vec2(mg.size) * imgScale);
    mg.noiseTex.unbind();
  }

  // ================== Terrain ======================== //

  if (CollapsingHeader("Terrain")) {
    SliderFloat("TESC divisions", &mg.tescDiv, 1.f, 1024.f);
    SliderFloat("Height multiplier", &mg.heightMultiplier, 0.1f, 100.f);

    Checkbox("Colorize chunks", &terrainPtr->showChunks);
    Checkbox("Show chunk normals", &terrainPtr->showChunkNormals);
    Checkbox("Attach camera", &terrainPtr->attachCam);

    Checkbox("Auto chunk size", &terrainPtr->autoChunkSize);
    SetItemTooltip("Will be half of the noise texture width");

    ivec2 coord = glm::floor(vec2(camera->position.x, camera->position.z) / terrainPtr->chunkSize);
    Text("Chunk00 coord {%d, %d}", coord.x, coord.y);

    BeginDisabled(terrainPtr->autoChunkSize);
    reGenTex |= SliderFloat("Chunk size", &terrainPtr->chunkSize, 1.f, 512.f);
    EndDisabled();

    reGenTex |= SliderInt("Chunks per axis", &terrainPtr->chunksPerAxis, 1, 10);
    reGenTex |= SliderInt("Chunk resolution", &terrainPtr->chunkResolution, 2, 20);

    if (TreeNode("Regions")) {
      for (size_t i = 0; i < mg.regions.size(); i++) {
        std::string name = mg.regions[i].uniformFmt;
        name.pop_back();
        name.pop_back();
        name = name.substr(2);

        reGenTex |= SliderFloat((name + " height").c_str(), &mg.regions[i].height, 0.f, 1.f);
        reGenTex |= ColorEdit3((name + " color").c_str(), glm::value_ptr(mg.regions[i].color));
      }

      TreePop();
    }
  }

  // ================== Character ====================== //

  if (CollapsingHeader("Character")) {
    if (!characterPtr) error("[gui] The character is not linked to gui");
    const vec3& pos = characterPtr->position;

    Checkbox("Fly mode", &characterPtr->flyMode);
    SliderFloat("Speed defulat", &characterPtr->speedDefault, 1.f, 1000.f);
    SliderFloat("Speed multiplier", &characterPtr->speedMul, 1.f, 1000.f);

    TextColored({0.f, 1.f, 1.f, 1.f}, "Position");
    Text("{%.2f, %.2f, %.2f}", pos.x, pos.y, pos.z);
  }
  // ================== Light ========================== //

  if (CollapsingHeader("Light")) {
    if (!lightPtr) error("[gui] The light is not linked to gui");
    SliderFloat("Radius", &lightPtr->radius, 0.f, 30.f);
    DragFloat3("Light position", glm::value_ptr(lightPtr->position));
  }

  // ================== Falloff ======================== //

  if (CollapsingHeader("Falloff")) {
    static float imgScale = 0.5f;

    reGenTex |= Checkbox("Enable", &mg.useFalloffmap);

    BeginDisabled(!mg.useFalloffmap);
    reGenTex |= SliderFloat("Parameter a", &mg.falloffA, 0.01f, 20.f);
    reGenTex |= SliderFloat("Parameter b", &mg.falloffB, 0.01f, 20.f);
    SliderFloat("Image scale##2", &imgScale, 0.01f, 1.f);

    mg.falloffTex.bind();
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    Image(mg.falloffTex.getId(), vec2(mg.size) * imgScale);
    mg.falloffTex.unbind();

    EndDisabled();
  }

  if (reGenTex) {
    terrainPtr->update(camera->getPosition(), true);
  }

  // ================== Other ========================== //

  if (CollapsingHeader("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);
    Checkbox("Wireframe mod", &global::drawWireframe);
  }

  End();
}


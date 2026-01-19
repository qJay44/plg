#include "Terrain.hpp"
#include <cassert>
#include <cstdlib>

#ifdef _WIN32
  #include <direct.h>
  #define CHDIR(p) _chdir(p);
#else
  #include <unistd.h>
  #define CHDIR(p) chdir(p);
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "gl/Shader.hpp"
#include "gl/Camera.hpp"
#include "gl/InputsHandler.hpp"
#include "gl/mesh/meshes.hpp"
#include "gui.hpp"
#include "utils/clrp.hpp"
#include "global.hpp"

void GLAPIENTRY MessageCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
) {
  switch (source) {
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return; // Handled by the Shader class itself
    case GL_DEBUG_SOURCE_API:
      return; // "SIMD32 shader inefficient", skipping since occurs only on my laptop
  }

  clrp::clrp_t clrpError;
  clrpError.attr = clrp::ATTRIBUTE::BOLD;
  clrpError.fg = clrp::FG::RED;
  fprintf(
    stderr, "GL CALLBACK: %s source = 0x%x, id = 0x%x type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), source, id, type, severity, clrp::format(message, clrpError).c_str()
  );
  exit(1);
}

int main() {
  CHDIR("../../../src");
  using global::window;
  using global::camera;

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  // Globals
  window = glfwCreateWindow(INIT_WIDTH, INIT_HEIGHT, "MyProgram", NULL, NULL);
  camera = new Camera({1031.f, 167.6f, 597.f}, {-0.73f, -0.42f, -0.44f}, 100.f);
  camera->setFarPlane(3000.f);
  camera->setSpeedDefault(100.f);

  assert(window);
  assert(camera);

  ivec2 winSize;
  glfwGetWindowSize(window, &winSize.x, &winSize.y);
  dvec2 winCenter = dvec2(winSize) / 2.;

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + 2 * !global::guiFocused);
  glfwSetCursorPos(window, winCenter.x, winCenter.y);

  // GLAD init
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, winSize.x, winSize.y);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("gl/shaders");
  glPatchParameteri(GL_PATCH_VERTICES, 4);

  Shader shaderMain("main.vert", "main.frag", "main.tesc", "main.tese");
  Shader shaderV4Color("v4_color.vert", "v4_color.frag");

  // ===== Inputs Handler ======================================= //

  glfwSetKeyCallback(window, InputsHandler::keyCallback);
  glfwSetCursorPosCallback(window, InputsHandler::cursorPosCallback);

  // ============================================================ //

  Mesh axis = meshes::axis(500000.f);

  Terrain terrain(camera->getPosition());

  gui::terrainPtr = &terrain;

  glEnable(GL_DEPTH_TEST);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static double titleTimer = glfwGetTime();
    static double prevTime = titleTimer + 1e-4;
    static double currTime = prevTime;

    constexpr float fpsLimit = 1.f / 120.f;
    currTime = glfwGetTime();
    global::dt = currTime - prevTime;

    // FPS cap
    if (global::dt < fpsLimit) continue;
    else prevTime = currTime;

    global::time += global::dt;

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
      InputsHandler::process();
      camera->update();
    } else
      glfwSetCursorPos(window, winCenter.x, winCenter.y);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      gui::fps = static_cast<u16>(1.f / global::dt);
      titleTimer = currTime;
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    terrain.update(camera->getPosition());
    terrain.draw(camera, shaderMain);

    if (global::drawGlobalAxis)
      axis.draw(camera, shaderV4Color);

    gui::draw();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  delete camera;

  return 0;
}


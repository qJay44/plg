#include "InputsHandler.hpp"

#include <cassert>

#include "../gui.hpp"
#include "imgui_impl_glfw.h"

using global::window;
using global::camera;

void InputsHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch (key) {
    case GLFW_KEY_R:
      if (action == GLFW_PRESS) {
        global::guiFocused = !global::guiFocused;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + 2 * !global::guiFocused);

        // Prevent moving camera towards cursor after disabling it
        dvec2 winCenter = global::getWinCenter();
        if (!global::guiFocused)
          glfwSetCursorPos(window, winCenter.x, winCenter.y);
      }
      break;
    case GLFW_KEY_E:
      if (action == GLFW_PRESS) gui::toggle();
      break;
    case GLFW_KEY_1:
      if (action == GLFW_PRESS && !global::guiFocused)
        global::drawWireframe = !global::drawWireframe;
      break;
    case GLFW_KEY_2:
      if (action == GLFW_PRESS && !global::guiFocused)
        global::drawGlobalAxis = !global::drawGlobalAxis;
      break;
  }
}

void InputsHandler::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  camera->moveByMouse({xpos, ypos});
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void InputsHandler::process() {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera->setSpeedMultiplier(5.f);
  else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    camera->setSpeedMultiplier(1.f);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->moveForward();
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->moveLeft();
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->moveBack();
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->moveRight();

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera->moveUp();
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera->moveDown();
}


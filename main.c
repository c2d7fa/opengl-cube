#include <GLFW/glfw3.h>

void render();

int main() {
  if (!glfwInit()) return 1;

  GLFWmonitor* fullscreen_monitor = NULL; // Windowed
  GLFWwindow* share = NULL;
  GLFWwindow* window = glfwCreateWindow(800, 800, "Cube", fullscreen_monitor, share);
  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);

  while (!glfwWindowShouldClose(window)) {
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void render() {
  glClearColor(0.1, 0.12, 0.2, 1);
  glClear(GL_COLOR_BUFFER_BIT);
}

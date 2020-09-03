#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
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
  glewInit();

  while (!glfwWindowShouldClose(window)) {
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void read_file(char* path, char* buffer, size_t max_size) {
  FILE* file = fopen(path, "r");

  size_t i = 0;
  while (i < max_size) {
    char ch = fgetc(file);
    if (ch <= 0) break;
    buffer[i++] = ch;
  }
  buffer[i++] = '\0';

  fclose(file);
}

void compile_shader_from_file(char* path, GLuint shader) {
  char* buffer = malloc(0x1000);
  read_file(path, buffer, 0x1000);

  const char* source = buffer;
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char* info_buffer = malloc(0x1000);
    glGetShaderInfoLog(shader, 0x1000, NULL, info_buffer);
    printf("Error compiling shader: %s\nThe shader was:\n%s", info_buffer, buffer);
  }

  free(buffer);
}

void render() {
  // Clear
  {
    glClearColor(0.1, 0.12, 0.2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  // Draw triangle. Centered, equilateral, side length 1.
  {
    // 0.5^2 + h^2 = 1^2
    const float x = 0.5;
    const float height = sqrt(1 * 1 - 0.5 * 0.5);
    const float y = height / 2;

    // Clockwise from bottom left
    float vertices[] = {
      -x, -y, 0,
       0,  y, 0,
       x, -y, 0,
    };

    // [TODO] We should not be doing most of this every frame!!
    unsigned int vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    unsigned int vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    unsigned int va_index = 0;
    glEnableVertexAttribArray(va_index);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    unsigned int va_component_size = 3;
    GLboolean va_is_normalized = GL_FALSE;
    unsigned int va_stride = 0;
    const void* va_start_offset = NULL;
    glVertexAttribPointer(va_index, va_component_size, GL_FLOAT, va_is_normalized, va_stride, va_start_offset);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    compile_shader_from_file("vertex.glsl", vertex_shader);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader_from_file("fragment.glsl", fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Only this is the actual drawing!
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }
}

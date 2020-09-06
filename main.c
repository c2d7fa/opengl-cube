#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// I'm following this tutorial: https://antongerdelan.net/opengl/
// Reference documentation: https://www.khronos.org/registry/OpenGL-Refpages/gl4/

struct context {
  unsigned int shader_program;
  unsigned int vao;
  unsigned int vao_index;
};

void render(struct context*);
void initialize(struct context*);

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

  struct context context;
  initialize(&context);

  while (!glfwWindowShouldClose(window)) {
    render(&context);

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

const unsigned int vertex_component_size = 3; // Number of floats needed to store each vertex in triangle
const unsigned int triangles = 2;             // Number of triangles used for cube

void initialize(struct context* context) {
  float vertices[] = {
    // Bottom left triangle, clockwise from bottom left
    -0.5, -0.5, 0.0,
    -0.5,  0.5, 0.0,
     0.5, -0.5, 0.0,

    // Top right triangle, clockwise from top left
    -0.5,  0.5, 0.0,
     0.5,  0.5, 0.0,
     0.5, -0.5, 0.0,
  };

  unsigned int vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

  glGenVertexArrays(1, &context->vao);

  glBindVertexArray(context->vao);
  context->vao_index = 0;
  glEnableVertexAttribArray(context->vao_index);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  GLboolean va_is_normalized = GL_FALSE;
  unsigned int va_stride = 0;
  const void* va_start_offset = NULL;
  glVertexAttribPointer(context->vao_index, vertex_component_size, GL_FLOAT, va_is_normalized, va_stride, va_start_offset);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  compile_shader_from_file("vertex.glsl", vertex_shader);
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  compile_shader_from_file("fragment.glsl", fragment_shader);

  context->shader_program = glCreateProgram();
  glAttachShader(context->shader_program, vertex_shader);
  glAttachShader(context->shader_program, fragment_shader);
  glLinkProgram(context->shader_program);
}

void render(struct context* context) {
  // Clear
  {
    glClearColor(0.1, 0.12, 0.2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  glUseProgram(context->shader_program);
  glBindVertexArray(context->vao);
  glDrawArrays(GL_TRIANGLES, context->vao_index, vertex_component_size * triangles);
}

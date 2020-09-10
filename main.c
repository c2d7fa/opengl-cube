#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #define GLEW_STATIC
    #pragma comment(lib, "glew32s.lib")
    #pragma comment(lib, "glfw3.lib")
    #pragma comment(lib, "opengl32.lib")
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "matrix.h"

struct context {
  unsigned int shader_program;
  unsigned int vao;
  GLFWwindow* window;
 
  unsigned int uniform_transform;
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
  glfwSwapInterval(0);
  glewInit();

  struct context context;
  context.window = window;
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

void link_shader_program(unsigned int program) {
    glLinkProgram(program);

    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        char* info_buffer = malloc(0x1000);
        glGetProgramInfoLog(program, 0x1000, NULL, info_buffer);
        printf("Error linking shader program: %s\n", info_buffer);
    }
}

const unsigned int triangles = 6 * 2;   // Number of triangles rendered

const unsigned int verticies_index = 0;
const unsigned int colors_index = 1;

void initialize(struct context* context) {
  glEnable(GL_DEPTH_TEST);

  float vertices[] = {
    // Front face
     0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,

    // Back face
     0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,
     0.5, -0.5, -0.5,
  };

  float vertex_colors[] = {
    1.0, 0.4, 0.6,
    1.0, 0.9, 0.2,
    0.7, 0.3, 0.8,
    0.5, 0.3, 1.0,

    0.2, 0.6, 1.0,
    0.6, 1.0, 0.4,
    0.6, 0.8, 0.8,
    0.4, 0.8, 0.8,
  };

  unsigned short triangle_indices[] = {
      // Front
      0, 1, 2,
      2, 3, 0,

      // Right
      0, 3, 7,
      7, 4, 0,

      // Bottom
      2, 6, 7,
      7, 3, 2,

      // Left
      1, 5, 6,
      6, 2, 1,

      // Back
      4, 7, 6,
      6, 5, 4,

      // Top
      5, 1, 0,
      0, 4, 5,
  };

  glGenVertexArrays(1, &context->vao);
  glBindVertexArray(context->vao);

  unsigned int triangles_ebo;
  glGenBuffers(1, &triangles_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangles_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof triangle_indices, triangle_indices, GL_STATIC_DRAW);

  unsigned int verticies_vbo;
  glGenBuffers(1, &verticies_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, verticies_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(verticies_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(verticies_index);

  unsigned int colors_vbo;
  glGenBuffers(1, &colors_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof vertex_colors, vertex_colors, GL_STATIC_DRAW);

  glVertexAttribPointer(colors_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(colors_index);

  // Unbind to prevent accidental modification
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  compile_shader_from_file("vertex.glsl", vertex_shader);
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  compile_shader_from_file("fragment.glsl", fragment_shader);

  context->shader_program = glCreateProgram();
  glAttachShader(context->shader_program, vertex_shader);
  glAttachShader(context->shader_program, fragment_shader);
  link_shader_program(context->shader_program);

  context->uniform_transform = glGetUniformLocation(context->shader_program, "transform");
}

// Based on https://antongerdelan.net/opengl/glcontext2.html
void update_fps(struct context* context) {
    // All times in seconds

    static double last_update_time = 0;
    static int frames_since_last_update = 0;

    double now = glfwGetTime();
    frames_since_last_update++;

    if (now - last_update_time > 0.25) {
        double fps = frames_since_last_update / (now - last_update_time);

        char title_buffer[128];
        sprintf(title_buffer, "Cube (%.1f FPS)", fps);
        glfwSetWindowTitle(context->window, title_buffer);

        last_update_time = now;
        frames_since_last_update = 0;
    }
}

float animation(float duration) {
    unsigned long int ms_time = glfwGetTime() * 1000;
    unsigned int ms_duration = duration * 1000;
    float ms_position = ms_time % ms_duration;

    return ms_position / ms_duration;
}

void render(struct context* context) {
  update_fps(context);

  // Clear
  {
    glClearColor(0.1, 0.12, 0.2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  glUseProgram(context->shader_program);

  struct mat4f transform = mat4f_identity;
  transform = mat4f_multiply(transform, mat4f_perspective());
  transform = mat4f_multiply(transform, mat4f_translation(0, 0, -3));
  transform = mat4f_multiply(transform, mat4f_rotate_x(0.15 * pi));
  transform = mat4f_multiply(transform, mat4f_rotate_y(2 * pi * animation(4)));
  glUniformMatrix4fv(context->uniform_transform, 1, GL_FALSE, mat4f_gl(transform));

  glBindVertexArray(context->vao);
  glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_SHORT, NULL);
}

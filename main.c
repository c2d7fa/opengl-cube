#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// I'm following this tutorial: https://antongerdelan.net/opengl/
// Reference documentation: https://www.khronos.org/registry/OpenGL-Refpages/gl4/ or https://docs.gl/

struct context {
  unsigned int shader_program;
  unsigned int vao;
  unsigned int ebo;

  unsigned int blue_uniform_location;

  GLFWwindow* window;
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

const unsigned int vertex_size = 2; // Number of floats needed to store each vertex in triangle
const unsigned int triangles = 2;   // Number of triangles rendered

void initialize(struct context* context) {
  float vertices[] = {
    -0.5, -0.5,
    -0.5,  0.5,
     0.5,  0.5,
     0.5, -0.5,
  };

  unsigned short triangle_indices[] = {
      0, 1, 2,
      0, 3, 2,
  };

  glGenVertexArrays(1, &context->vao);
  glBindVertexArray(context->vao);

  glGenBuffers(1, &context->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof triangle_indices, triangle_indices, GL_STATIC_DRAW);

  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

  const int index = 0;
  const GLboolean va_is_normalized = GL_FALSE;
  const unsigned int va_stride = 0;
  const void* va_start_offset = NULL;
  // Note: 'ebo' and 'vbo' are bound.
  glVertexAttribPointer(index, vertex_size, GL_FLOAT, va_is_normalized, va_stride, va_start_offset);
  glEnableVertexAttribArray(index);

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

  context->blue_uniform_location = glGetUniformLocation(context->shader_program, "blue");
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
  glUniform1f(context->blue_uniform_location, fabs(0.5 - animation(4.0)) * 2.0);

  glBindVertexArray(context->vao);
  glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_SHORT, NULL);
}

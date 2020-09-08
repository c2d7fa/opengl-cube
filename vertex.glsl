#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vertex_color;

uniform mat4 transform;

out vec3 color;

void main() {
  gl_Position = transform * vec4(pos, 1.0);
  color = vertex_color;
}

#version 460

uniform float blue;

out vec4 color;

void main() {
  // Note: Square is drawn between (200, 200) and (600, 600).
  color = vec4((gl_FragCoord.x - 200) / (400 * 2) + 0.5, (gl_FragCoord.y - 200) / (400 * 2) + 0.5, blue, 1.0);
}

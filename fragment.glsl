#version 460

out vec4 color;

void main() {
  // Note: Square is drawn between (200, 200) and (600, 600).
  color = vec4((gl_FragCoord.x - 200) / (400 * 2) + 0.5, (gl_FragCoord.y - 200) / (400 * 2) + 0.5, 1 - ((gl_FragCoord.x - 200) / (400 * 3)), 1.0);
}

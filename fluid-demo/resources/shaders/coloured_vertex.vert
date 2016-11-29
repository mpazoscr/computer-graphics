#version 400

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 col;

uniform mat4 view;
uniform mat4 projection;

out vec4 colour;

void main ()
{
  gl_Position = projection * view * vec4 (pos, 1.0);
  colour = col;
}

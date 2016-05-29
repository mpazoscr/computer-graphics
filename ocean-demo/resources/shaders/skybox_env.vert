#version 400

layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 n;

uniform mat4 view;
uniform mat4 projection;

out vec3 position;
out vec4 normal;

void main ()
{
  gl_Position = projection * view * vec4 (vp, 1.0);
  position = vp;
  normal =  vec4(n, 0.0);
}

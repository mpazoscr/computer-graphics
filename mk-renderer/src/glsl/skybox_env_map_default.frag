#version 400

uniform samplerCube environmentMap;

in vec3 position;

out vec4 frag_colour;

void main ()
{
  frag_colour = texture(environmentMap, position);
}

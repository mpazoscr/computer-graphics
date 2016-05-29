#version 400

uniform vec3 sunDir;
uniform vec3 eyePos;

in vec3 position;
in vec3 normal;

out vec4 frag_colour;

void main ()
{
  vec3 blue = vec3(0.0, 0.2, 0.8);
  vec3 orange = vec3(0.9, 0.6, 0.2);
  vec3 sunColor = vec3(1.0, 1.0, 0.8);

  vec3 fragPosInEyeCoords = normalize(position - eyePos);

  float skyFactor = smoothstep(sunDir.y - 1.5, sunDir.y + 1.3, fragPosInEyeCoords.y);
  float sunFactor = pow(clamp(dot(fragPosInEyeCoords, sunDir), 0.0, 1.0), 200.0);

  frag_colour = vec4(sunColor * sunFactor + mix(orange, blue, skyFactor) * (1.0 - sunFactor), 1.0);
}

#version 410

uniform mat4 model;
uniform mat4 projection;

in vec4 vPosition;
in vec3 vColor;

out vec4 color;

void main()
{
    gl_Position = projection*model*vPosition;
    color = vec4(vColor,1.0);
}

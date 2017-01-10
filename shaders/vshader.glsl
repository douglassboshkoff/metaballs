#version 410

uniform vec3 vColor;
uniform mat4 model;
uniform mat4 projection;

in vec4 vPosition;

out vec4 color;
void main()
{
    gl_Position = projection*model*vPosition;
    color = vec4(vColor,1.0);
}

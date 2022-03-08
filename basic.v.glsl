/**
 * Basic vertex shader that passes color for interpolation and sets x/y/z
 * position coordinates.
 */

#version 140

in vec3 position;
in vec3 color;
varying vec3 v_color;

void main() {
    v_color = color;
    gl_Position = vec4(position.x, position.y, position.z, 1);
}

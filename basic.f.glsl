/**
 * Basic fragment shader that interpolates vertex colors
 */

#version 140

varying vec3 v_color;

void main() {
    gl_FragColor = vec4(v_color.r, v_color.g, v_color.b, 1.0);
}

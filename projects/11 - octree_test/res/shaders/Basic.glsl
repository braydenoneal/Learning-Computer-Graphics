// shader vertex
#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in float brightness;

out float f_Brightness;

uniform mat4 u_view_projection;

void main() {
    gl_Position = u_view_projection * position;
    f_Brightness = brightness;
}

// shader fragment
#version 330 core

layout (location = 0) out vec4 color;

in float f_Brightness;

uniform sampler2DArray u_Textures;

void main() {
    color = vec4(0.5f, 0.5f, 0.5f, 1.0f) * f_Brightness;
}

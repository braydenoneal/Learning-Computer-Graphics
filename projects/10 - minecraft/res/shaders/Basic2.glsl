// shader vertex
#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 textureCoordinates;
layout (location = 2) in float brightness;

out vec3 v_TextureCoordinates;
out float f_Brightness;

uniform mat4 u_mt;
uniform mat4 u_all;

void main() {
    gl_Position = u_all * u_mt * position;
    v_TextureCoordinates = textureCoordinates;
    f_Brightness = brightness;
}

// shader fragment
#version 330 core

layout (location = 0) out vec4 color;

in vec3 v_TextureCoordinates;
in float f_Brightness;

uniform sampler2DArray u_Textures;

void main() {
    vec4 textureColor = texture(u_Textures, v_TextureCoordinates);
    float fog_distance = gl_FragCoord.z / gl_FragCoord.w;
    textureColor.rgb *= f_Brightness;
//    textureColor = mix(textureColor, vec4(141.0f / 255.0f, 159.0f / 255.0f, 1.0f, 1.0f), (1.0 - exp2(-0.0005 * fog_distance * fog_distance)));
    color = textureColor;
}

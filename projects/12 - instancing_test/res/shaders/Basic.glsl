// shader vertex
#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 u_view_projection;
uniform vec3 u_light_position;

out vec3 v_color;
out vec3 v_normal;
out vec3 v_position;
out vec3 v_light_position;

void main() {
    gl_Position = u_view_projection * vec4(position.x, position.y, position.z, position.w);
    float brightness = abs(position.y) / 64.0f;
    v_color = vec3((int(position.x) % 16) / 16.0f, brightness, (int(position.z) % 16) / 16.0f);
    v_color = vec3(0.6f, 0.4f, 0.2f);
    v_normal = normal;
    v_position = vec3(position.x, position.y, position.z);
    v_light_position = u_light_position;
}

// shader fragment
#version 330 core
layout (location = 0) out vec4 color;

in vec3 v_color;
in vec3 v_normal;
in vec3 v_position;
in vec3 v_light_position;

void main() {
    vec3 light_direction = normalize(v_light_position - v_position);
    float difference = max(dot(v_normal, light_direction), 0.0);
    vec3 result = v_color * difference;
    color = vec4(result + vec3(0.1f, 0.1f, 0.15f), 1.0f);
}

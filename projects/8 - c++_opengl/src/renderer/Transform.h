#pragma once

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 translate (glm::vec3 vec3) {
    return glm::translate(glm::mat4(1.0f), vec3);
}

glm::mat4 rotate (glm::vec3 angle) {
    auto vec3 = glm::mat4(1.0f);

    vec3 = glm::rotate(vec3, angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
    vec3 = glm::rotate(vec3, angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
    vec3 = glm::rotate(vec3, angle.z, glm::vec3(0.0f, 0.0f, 1.0f));

    return vec3;
}

glm::mat4 render_transformation (glm::vec3 camera_position, glm::vec3 camera_angle, glm::vec3 model_position, glm::vec3 model_angle) {
    glm::mat4 camera_rotate = rotate(camera_angle);
    glm::mat4 camera_translate = translate(glm::vec3(-camera_position.x, -camera_position.y, camera_position.z));
    glm::mat4 model_translate = translate(model_position);
    glm::mat4 model_rotate = rotate(model_angle);

    return camera_rotate * camera_translate * model_translate * model_rotate;
}

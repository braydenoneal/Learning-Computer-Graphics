#include <glm/vec3.hpp>
#include <list>

class GameContext {
public:
    glm::vec3 camera_position{glm::vec3(0.0f, 32.675f, -5.0f)};
    glm::vec3 camera_angle{glm::vec3(0.0f)};
    glm::vec3 model_position{glm::vec3(0.0f)};
    glm::vec3 model_angle{glm::vec3(0.0f)};
    int cube_count{4};
    bool paused{true};
    double default_cursor_x{0.0};
    double default_cursor_y{0.0};
    double camera_cursor_x{0.0};
    double camera_cursor_y{0.0};
};

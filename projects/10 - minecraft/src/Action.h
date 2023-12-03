class Action {
public:
    static void do_action(int key_code, WindowContext *window_context, GameContext *game_context) {
        switch (key_code) {
            case GLFW_KEY_W:
                game_context->camera_position = Transform::translate_in_direction_by_amount(game_context->camera_position, game_context->camera_angle.y, glm::vec3(0.0f, 0.0f, 0.1f));
                break;
            case GLFW_KEY_S:
                game_context->camera_position = Transform::translate_in_direction_by_amount(game_context->camera_position, game_context->camera_angle.y, glm::vec3(0.0f, 0.0f, -0.1f));
                break;
            case GLFW_KEY_D:
                game_context->camera_position = Transform::translate_in_direction_by_amount(game_context->camera_position, game_context->camera_angle.y, glm::vec3(0.1f, 0.0f, 0.0f));
                break;
            case GLFW_KEY_A:
                game_context->camera_position = Transform::translate_in_direction_by_amount(game_context->camera_position, game_context->camera_angle.y, glm::vec3(-0.1f, 0.0f, 0.0f));
                break;
            case GLFW_KEY_SPACE:
                game_context->camera_position.y += 0.1f;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                game_context->camera_position.y -= 0.1f;
                break;
            default:
                break;
        }
    }
};

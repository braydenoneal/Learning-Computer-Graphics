class Draw {
public:
    WindowContext *window_context;
    GameContext *game_context;
    Cube *cube;

    Draw(WindowContext *input_window_context, GameContext *input_game_context, Cube *input_cube) {
        window_context = input_window_context;
        game_context = input_game_context;
        cube = input_cube;
    }

    void draw(int x, int z) const {
        glm::mat4 perspective = Transform::perspective_transformation(window_context->field_of_view, window_context->get_aspect_ratio(), 0.0001f, 10000.0f);
//        cube->draw(perspective, game_context->cube_count, game_context->camera_position, game_context->camera_angle, game_context->model_position, game_context->model_angle);
        cube->static_draw(perspective, game_context->camera_position, game_context->camera_angle, game_context->model_position, game_context->model_angle, x, z);
    }
};

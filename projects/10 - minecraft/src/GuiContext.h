class GuiContext {
public:
    WindowContext *window_context;
    GameContext *game_context;

    GuiContext(WindowContext *input_window_context, GameContext *input_game_context) {
        window_context = input_window_context;
        game_context = input_game_context;

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window_context->window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
        ImGui::StyleColorsDark();
    }

    void render() const {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Debug");
            ImGui::SliderFloat3("Position", &game_context->camera_position[0], -10.0f, 10.0f);
            ImGui::SliderInt("Cubes", &game_context->cube_count, 1, 256);
            ImGui::SliderFloat("FOV", &window_context->field_of_view, 1.0f, 120.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    static void terminate() {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

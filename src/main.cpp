#include "config.h"

#include "source/render/texture.hpp"
#include "skarabeusz.hpp"
#include "connection.hpp"


const int SCREEN_WIDTH{ 1024 };
const int SCREEN_HEIGHT{ 768 };

class Scene {
protected:
    SDL_Renderer* renderer;
    bool quit;

public:
    Scene(SDL_Renderer* rend) : renderer(rend), quit(false) {}

    virtual void start() {};

    virtual void handleEvents(SDL_Event& event) {};

    virtual void update() {};
    virtual void render() {};

    void quitScene() {
        quit = true;
    }

    virtual void run() {
        start();

        while (!quit) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                handleEvents(event);
            }
            update();

            render();
        }
    }

    virtual ~Scene() {}
};

class Game : public Scene {
protected:
    mathy::vec2<int> mouse_pos = mathy::vec2<int>::ZERO();
    bool mouse_left_down{ false };
    Uint8 mouse_state{};

    bool first_move{ true };

    render::texture bg = render::texture("res/bg.png", mathy::vec2<int>{512, 384}, mathy::vec2<int>{1024, 768}, 0.0f, renderer);

    render::texture flap = render::texture("res/x.png", mathy::vec2<int>{512, 384}, mathy::vec2<int>{1024, 1024}, 0.0f, renderer);

    Skarabeusz skarabeusze[25] = {
        Skarabeusz(renderer, mathy::vec2<int>{198, 168}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{334, 168}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{511, 130}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{690, 166}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{130, 246}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{265, 243}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{410, 248}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{510, 235}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{614, 248}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{758, 248}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{245, 393}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{440, 394}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{509, 330}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{582, 398}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{782, 393}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{275, 565}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{414, 562}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{509, 486}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{513, 613}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{612, 557}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{749, 562}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{880, 566}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{341, 642}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{682, 636}, mathy::vec2<int>{80, 95}, 0.0f),
        Skarabeusz(renderer, mathy::vec2<int>{824, 642}, mathy::vec2<int>{80, 95}, 0.0f)
    };

    size_t skarabeusze_size{ sizeof(skarabeusze) / sizeof(skarabeusze[0]) };

    std::vector<Line> connection_lines;

    int actual_skarabeusz_index{};
    int previous_skarabeusz_index{25};

    std::vector<std::string> connections;
    bool can_move{ true };

public:
    Game(SDL_Renderer* rend) : Scene(rend) {}

    virtual void start() override {
        flap.enabled = false;

        skarabeusze[0].neighbours_indexes = { 4, 5 };
        skarabeusze[1].neighbours_indexes = { 5, 6 };
        skarabeusze[2].neighbours_indexes = { 6, 8 };
        skarabeusze[3].neighbours_indexes = { 8, 9 };
        skarabeusze[4].neighbours_indexes = { 0, 5 };
        skarabeusze[5].neighbours_indexes = { 0, 1, 4, 6, 10 };
        skarabeusze[6].neighbours_indexes = { 1, 2, 7, 11, 10, 5 };
        skarabeusze[7].neighbours_indexes = { 6, 8 };
        skarabeusze[8].neighbours_indexes = { 2, 3, 7, 9, 13 };
        skarabeusze[9].neighbours_indexes = { 3, 8, 14 };
        skarabeusze[10].neighbours_indexes = { 5, 6, 11, 15 };
        skarabeusze[11].neighbours_indexes = { 10, 6, 12, 17, 16, 15 };
        skarabeusze[12].neighbours_indexes = { 11, 13 };
        skarabeusze[13].neighbours_indexes = { 12, 17, 19, 20, 14, 8 };
        skarabeusze[14].neighbours_indexes = { 13, 8, 9, 20 };
        skarabeusze[15].neighbours_indexes = { 10, 11, 16, 22 };
        skarabeusze[16].neighbours_indexes = { 15, 11, 18, 22 };
        skarabeusze[17].neighbours_indexes = { 11, 13 };
        skarabeusze[18].neighbours_indexes = { 16, 19 };
        skarabeusze[19].neighbours_indexes = { 18, 13, 20, 23 };
        skarabeusze[20].neighbours_indexes = { 19, 13, 14, 21, 24, 23 };
        skarabeusze[21].neighbours_indexes = { 20, 24 };
        skarabeusze[22].neighbours_indexes = { 15, 16 };
        skarabeusze[23].neighbours_indexes = { 19, 20 };
        skarabeusze[24].neighbours_indexes = { 20, 21};

        for (int i = 0; i < skarabeusze_size; i++) {
            skarabeusze[i].state = selected;
        }
    }

    virtual void handleEvents(SDL_Event& event) override {
        if (event.type == SDL_QUIT) {
            quitScene();
        }
        else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            quitScene();
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouse_left_down = true;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouse_left_down = false;
            }
        }
    }

    virtual void update() override {
        mouse_state = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

        for (int i = 0; i < skarabeusze_size; i++) {
            if (mathy::distance(mouse_pos, skarabeusze[i].position) < ((skarabeusze[i].size.x + skarabeusze[i].size.y) / 8) && mouse_state && mouse_left_down) {
                if (skarabeusze[i].can_select || first_move) {
                    if (i != previous_skarabeusz_index) {
                        int temp_actual_skarabeusz_index = actual_skarabeusz_index;
                        int temp_previos_skarabeusz_index = previous_skarabeusz_index;
                        previous_skarabeusz_index = actual_skarabeusz_index;
                        actual_skarabeusz_index = i;
                        std::string actual_connection1 = std::to_string(previous_skarabeusz_index) + "" + std::to_string(actual_skarabeusz_index);
                        std::string actual_connection2 = std::to_string(actual_skarabeusz_index) + "" + std::to_string(previous_skarabeusz_index);
                        for (int c = 0; c < connections.size(); c++) {
                            if (actual_connection1 == connections[c] || actual_connection2 == connections[c]) {
                                actual_skarabeusz_index = temp_actual_skarabeusz_index;
                                previous_skarabeusz_index = temp_previos_skarabeusz_index;
                                break;
                            }
                        }

                        if (!first_move)
                            connection_lines.push_back(Line(skarabeusze[actual_skarabeusz_index].position, skarabeusze[previous_skarabeusz_index].position));

                        skarabeusze[i].state = confirmed;
                        std::cout << "actual: " << i << "\n previos: " << previous_skarabeusz_index << '\n';
                        std::cout << "actual connection: " << actual_connection1 << " AND: " << actual_connection2 << '\n';
                        first_move = false;
                        skarabeusze[i].was_confirmed = true;
                        connections.push_back(actual_connection1);
                        connections.push_back(actual_connection2);

                        for (int j = 0; j < skarabeusze_size; j++) {
                            skarabeusze[j].can_select = false;
                            if (!skarabeusze[j].was_confirmed) {
                                skarabeusze[j].state = empty;
                            }
                            else {
                                skarabeusze[j].state = confirmed;
                            }
                        }

                        for (int v = 0; v < connections.size(); v++) {
                            std::cout << "connection nr." << v << ": " << connections[v] << '\n';
                        }
                    }
                }
            }
        }

        for (int i = 0; i < skarabeusze[actual_skarabeusz_index].neighbours_indexes.size(); i++) {
            if (!first_move) {
                skarabeusze[skarabeusze[actual_skarabeusz_index].neighbours_indexes[i]].can_select = true;
                if (skarabeusze[actual_skarabeusz_index].neighbours_indexes[i] != previous_skarabeusz_index)
                    skarabeusze[skarabeusze[actual_skarabeusz_index].neighbours_indexes[i]].state = selected;
            }
        }

        // CHECK IF PLAYER CAN MOVE

        //if (mouse_left_down)
        //    std::cout << "x: " << mouse_pos.x << " y : " << mouse_pos.y << '\n';

        // TODO: POŁĄCZENIA 

        /*bool all_confirmed = true;

        for (const auto& skarabeusz : skarabeusze) {
            if (skarabeusz.state != empty) {
                all_confirmed = false;
                break;
            }
        }

        if (all_confirmed) {
            flap.enabled = true;
            skarabeusze[12].enabled = false;
        }*/
    }

    virtual void render() override {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        bg.render_texture();
        flap.render_texture();

        for (int i = 0; i < connection_lines.size(); i++) {
            connection_lines[i].render_line(renderer);
        }

        for (int i = 0; i < skarabeusze_size; i++) {
            skarabeusze[i].render();
        }

        SDL_RenderPresent(renderer);
    }
};

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("YUMESDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Game gameScene(renderer);

    Scene* currentScene = &gameScene;
    currentScene->run();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

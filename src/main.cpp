
#include <SFML/Graphics.hpp>

#include "Game.h"

static sf::RenderWindow * main_window;

void resized(const sf::Event::SizeEvent & size)
{
    main_window->GetDefaultView().SetHalfSize(size.Width / 2, size.Height / 2);
}


int main()
{
    main_window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "grinch");
    main_window->UseVerticalSync(true);

    Game * game = new Game;
    long long saved_state_frame_index = -1;
    std::vector<byte> saved_state;
    std::vector<std::vector<byte>*> rewind_buffer;

    bool frame_advance_mode = false;
    while (main_window->IsOpened()) {
        sf::Event event;
        bool do_frame = !frame_advance_mode;
        while (main_window->GetEvent(event)) {
            switch (event.Type) {
                case sf::Event::Closed:
                    main_window->Close();
                    break;
                case sf::Event::Resized:
                    resized(event.Size);
                    break;
                case sf::Event::KeyPressed: {
                    // check for closing
                    bool alt_f4 = event.Key.Alt && event.Key.Code == sf::Key::F4;
                    if (event.Key.Code == sf::Key::Escape || alt_f4) {
                        main_window->Close();
                        goto break_main_loop;
                    }
                    // check for engine hax
                    switch (event.Key.Code) {
                        case sf::Key::F12:
                            frame_advance_mode = !frame_advance_mode;
                            break;
                        case sf::Key::Space:
                            do_frame = true;
                            break;
                        case sf::Key::F1:
                            saved_state.clear();
                            game->saveState(&saved_state);
                            saved_state_frame_index = game->frame_counter;
                            break;
                        case sf::Key::F2: {
                            if (!saved_state.empty()) {
                                std::vector<byte>::const_iterator buffer = saved_state.begin();
                                game->loadState(&buffer);
                                int frame_counter = (int)game->frame_counter;
                                for (int i = frame_counter; i < (int)rewind_buffer.size(); i++)
                                    delete rewind_buffer[i];
                                rewind_buffer.resize(frame_counter);
                            }
                            break;
                        }
                        default:;
                    }
                    break;
                }
                default:;
            }
        }

        if (do_frame) {
            if (main_window->GetInput().IsKeyDown(sf::Key::Back)) {
                // rewind
                if (rewind_buffer.size() != 0) {
                    std::vector<byte>* last_state = rewind_buffer[rewind_buffer.size() - 1];
                    std::vector<byte>::const_iterator buffer = last_state->begin();
                    game->loadState(&buffer);
                    delete last_state;
                    rewind_buffer.resize(rewind_buffer.size() - 1);
                    if (saved_state_frame_index >= game->frame_counter)
                        saved_state.clear();
                }
            } else {
                // forwards
                game->doFrame(&main_window->GetInput());
                std::vector<byte>* state = new std::vector<byte>;
                game->saveState(state);
                rewind_buffer.push_back(state);
            }
        }

        main_window->Clear();

        game->render(main_window);

        main_window->Display();
    }
    break_main_loop:
    return 0;
}


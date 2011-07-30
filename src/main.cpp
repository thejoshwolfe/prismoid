
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
    game->entity = new Entity(sf::Vector2f(0, 0), sf::Vector2f(30, 30), sf::Vector2f(0, 0));
    std::vector<byte> state;

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
                            state.clear();
                            game->saveState(&state);
                            break;
                        case sf::Key::F2: {
                            std::vector<byte>::const_iterator buffer = state.begin();
                            game->loadState(&buffer);
                            break;
                        }
                        default:;
                    }
                    break;
                }
                default:;
            }
        }

        if (do_frame)
            game->doFrame(&main_window->GetInput());

        main_window->Clear();

        game->render(main_window);

        main_window->Display();
    }
    break_main_loop:
    return 0;
}


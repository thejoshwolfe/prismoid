
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

    while (main_window->IsOpened()) {
        sf::Event event;
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
                    break;
                }
                default:;
            }
        }

        game->doFrame(&main_window->GetInput());

        main_window->Clear();

        game->render(main_window);

        main_window->Display();
    }
    break_main_loop:
    return 0;
}


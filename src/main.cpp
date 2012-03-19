
#include <SFML/Graphics.hpp>

#include "Game.h"

// non-inline functions for debugger evaluation
const char * s(bigint x) { return x.get_str().c_str();}
const char * s(bigfraction x) { return x.get_str().c_str();}
const char * s(Vector2 v) { return ("(" + v.x.get_str() + "," + v.y.get_str() + ")").c_str(); }
const char * s(Vector2q v) { return ("(" + v.x.get_str() + "," + v.y.get_str() + ")").c_str(); }
const char * s(Vector3 v) { return ("(" + v.x.get_str() + "," + v.y.get_str() + "," + v.z.get_str() + ")").c_str(); }
const char * s(Edge e) { return ("((" + e.points[0].x.get_str() + "," + e.points[0].y.get_str() + "," + e.points[0].z.get_str() + "), " +
                                  "(" + e.points[1].x.get_str() + "," + e.points[1].y.get_str() + "," + e.points[1].z.get_str() + "))").c_str(); }

int main(int argc, char ** argv)
{
    std::string map = "resources/test.tmx";
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.find("--map=") == 0) {
            map = arg.substr(6);
        }
    }

    sf::RenderWindow * main_window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "grinch");
    main_window->UseVerticalSync(true);

    Game * game = new Game(map);

    while (main_window->IsOpened()) {
        sf::Event event;
        while (main_window->GetEvent(event)) {
            switch (event.Type) {
                case sf::Event::Closed:
                    main_window->Close();
                    break;
                case sf::Event::Resized:
                    main_window->GetDefaultView().SetHalfSize(event.Size.Width / 2, event.Size.Height / 2);
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


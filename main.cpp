
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow * main_window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "grinch");
    main_window->UseVerticalSync(true);

    while (main_window->IsOpened()) {
        sf::Event event;
        while (main_window->GetEvent(event)) {
            switch (event.Type) {
                case sf::Event::Closed:
                    main_window->Close();
                    break;
                case sf::Event::Resized:
                    glViewport(0, 0, event.Size.Width, event.Size.Height);
                    break;
                case sf::Event::KeyPressed:
                    // only check for closing here
                    if (event.Key.Code == sf::Key::Escape || (event.Key.Alt && event.Key.Code == sf::Key::F4))
                        main_window->Close();
                    break;
            }
        }

        main_window->Clear();

        main_window->Draw(sf::Shape::Line(10, 10, 710, 100, 15, sf::Color::Red));
        main_window->Draw(sf::Shape::Circle(200, 200, 100, sf::Color::Yellow, 10, sf::Color::Blue));
        main_window->Draw(sf::Shape::Rectangle(350, 200, 600, 350, sf::Color::Green));

        main_window->Display();
    }
    return 0;
}


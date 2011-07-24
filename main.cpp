
#include <SFML/Graphics.hpp>

static sf::RenderWindow * main_window;

void resized(const sf::Event::SizeEvent & size)
{
    sf::View * default_view = &main_window->GetDefaultView();
    default_view->SetHalfSize(size.Width / 2, size.Height / 2);
}

int main()
{
    main_window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "grinch");
    main_window->UseVerticalSync(true);

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
                case sf::Event::KeyPressed:
                    // only check for closing here
                    if (event.Key.Code == sf::Key::Escape || (event.Key.Alt && event.Key.Code == sf::Key::F4))
                        main_window->Close();
                    break;
            }
        }

        const sf::Input * input = &main_window->GetInput();
        if (input->IsKeyDown(sf::Key::Up)) main_window->GetDefaultView().Move(0, -5);
        if (input->IsKeyDown(sf::Key::Left)) main_window->GetDefaultView().Move(-5, 0);
        if (input->IsKeyDown(sf::Key::Down)) main_window->GetDefaultView().Move(0, 5);
        if (input->IsKeyDown(sf::Key::Right)) main_window->GetDefaultView().Move(5, 0);

        if (input->IsKeyDown(sf::Key::Comma)) main_window->GetDefaultView().Zoom(0.9f);
        if (input->IsKeyDown(sf::Key::Period)) main_window->GetDefaultView().Zoom(1.0f / 0.9f);

        main_window->Clear();

        main_window->Draw(sf::Shape::Line(10, 10, 710, 100, 15, sf::Color::Red));
        main_window->Draw(sf::Shape::Circle(200, 200, 100, sf::Color::Yellow, 10, sf::Color::Blue));
        main_window->Draw(sf::Shape::Rectangle(350, 200, 600, 350, sf::Color::Green));

        main_window->Display();
    }
    return 0;
}


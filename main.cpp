
#include <SFML/Graphics.hpp>

static sf::RenderWindow * main_window;

void resized(const sf::Event::SizeEvent & size)
{
    main_window->GetDefaultView().SetHalfSize(size.Width / 2, size.Height / 2);
}

static float position_x = 0;
static float position_y = 0;
static float velocity_x = 0;
static float velocity_y = 0;

int main()
{
    main_window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "grinch");
    main_window->UseVerticalSync(true);

    sf::Image * image = new sf::Image();
    if (!image->LoadFromFile("sample.png"))
        return 1;
    sf::Sprite * sprite = new sf::Sprite(*image);
    sprite->SetCenter(image->GetWidth() / 2, image->GetHeight() / 2);
    sprite->SetPosition(10, 10);
    sprite->SetScale(1.0f / 8, 1.0f / 8);

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
                    // only check for closing here
                    bool alt_f4 = event.Key.Alt && event.Key.Code == sf::Key::F4;
                    if (event.Key.Code == sf::Key::Escape || alt_f4) {
                        main_window->Close();
                        goto break_main_loop;
                    }
                    break;
                }
            }
        }

        float elapsed_time = main_window->GetFrameTime();

        const sf::Input * input = &main_window->GetInput();
        if (input->IsKeyDown(sf::Key::W)) velocity_y -= elapsed_time * 2000.0f;
        if (input->IsKeyDown(sf::Key::A)) velocity_x -= elapsed_time * 2000.0f;
        if (input->IsKeyDown(sf::Key::S)) velocity_y += elapsed_time * 2000.0f;
        if (input->IsKeyDown(sf::Key::D)) velocity_x += elapsed_time * 2000.0f;
        position_x += elapsed_time * velocity_x;
        position_y += elapsed_time * velocity_y;
        sprite->SetPosition(position_x, position_y);

        sf::View * view = &main_window->GetDefaultView();
        view->SetCenter(position_x, position_y);
        if (input->IsKeyDown(sf::Key::Comma)) view->Zoom(0.98f);
        if (input->IsKeyDown(sf::Key::Period)) view->Zoom(1.0f / 0.98f);

        main_window->Clear();

        main_window->Draw(sf::Shape::Line(10, 10, 710, 100, 15, sf::Color::Red));
        main_window->Draw(sf::Shape::Circle(200, 200, 100, sf::Color::Yellow, 10, sf::Color::Blue));
        main_window->Draw(sf::Shape::Rectangle(350, 200, 600, 350, sf::Color::Green));
        main_window->Draw(*sprite);

        main_window->Display();
    }
    break_main_loop:
    return 0;
}


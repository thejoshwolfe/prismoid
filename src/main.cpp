
#include <SFML/Graphics.hpp>

#include "Entity.h"

static sf::RenderWindow * main_window;

void resized(const sf::Event::SizeEvent & size)
{
    main_window->GetDefaultView().SetHalfSize(size.Width / 2, size.Height / 2);
}

int main()
{
    main_window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "grinch");
    main_window->UseVerticalSync(true);

    Entity * entity = new Entity(sf::Vector2f(0, 0), sf::Vector2f(30, 30));

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
                default:;
            }
        }

        float elapsed_time = main_window->GetFrameTime();

        const sf::Input * input = &main_window->GetInput();
        if (input->IsKeyDown(sf::Key::W)) entity->velocity.y -= elapsed_time * 2000.0f;
        if (input->IsKeyDown(sf::Key::A)) entity->velocity.x -= elapsed_time * 2000.0f;
        if (input->IsKeyDown(sf::Key::S)) entity->velocity.y += elapsed_time * 2000.0f;
        if (input->IsKeyDown(sf::Key::D)) entity->velocity.x += elapsed_time * 2000.0f;
        entity->center.x += elapsed_time * entity->velocity.x;
        entity->center.y += elapsed_time * entity->velocity.y;

        sf::View * view = &main_window->GetDefaultView();
        view->SetCenter(entity->center.x, entity->center.y);
        if (input->IsKeyDown(sf::Key::Comma)) view->Zoom(0.98f);
        if (input->IsKeyDown(sf::Key::Period)) view->Zoom(1.0f / 0.98f);

        main_window->Clear();

        const float floor = 700;
        main_window->Draw(sf::Shape::Line(0, floor, 500, floor, 1, sf::Color::Red));
        sf::Drawable * drawable = entity->toDrawable();
        main_window->Draw(*drawable);
        delete drawable;

        main_window->Display();
    }
    break_main_loop:
    return 0;
}


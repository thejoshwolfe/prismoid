
#include <SFML/Graphics.hpp>

#include "Entity.h"

static sf::RenderWindow * main_window;
static const float floor_y = 300;

void resized(const sf::Event::SizeEvent & size)
{
    main_window->GetDefaultView().SetHalfSize(size.Width / 2, size.Height / 2);
}

void doFrame(Entity * entity)
{
    const sf::Input * input = &main_window->GetInput();
    float move_acceleration = 0.4f;
    if (input->IsKeyDown(sf::Key::W)) entity->velocity.y -= move_acceleration;
    if (input->IsKeyDown(sf::Key::A)) entity->velocity.x -= move_acceleration;
    if (input->IsKeyDown(sf::Key::S)) entity->velocity.y += move_acceleration;
    if (input->IsKeyDown(sf::Key::D)) entity->velocity.x += move_acceleration;

    sf::View * view = &main_window->GetDefaultView();
    float zoom_speed = 0.999f;
    if (input->IsKeyDown(sf::Key::Comma)) view->Zoom(zoom_speed);
    if (input->IsKeyDown(sf::Key::Period)) view->Zoom(1.0f / zoom_speed);

    float gravity = 0.2;
    entity->velocity.y += gravity;
    entity->center.x += entity->velocity.x;
    entity->center.y += entity->velocity.y;
    if (entity->center.y >= floor_y) {
        // collision with ground
        entity->center.y = floor_y;
        entity->velocity.y *= -0.5;
    }

    view->SetCenter(entity->center.x, entity->center.y);
}

int main()
{
    main_window = new sf::RenderWindow(sf::VideoMode(800, 600, 32), "grinch");
    main_window->UseVerticalSync(true);

    Entity * entity = new Entity(sf::Vector2f(0, 0), sf::Vector2f(30, 30));

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
                    // only check for closing here
                    bool alt_f4 = event.Key.Alt && event.Key.Code == sf::Key::F4;
                    if (event.Key.Code == sf::Key::Escape || alt_f4) {
                        main_window->Close();
                        goto break_main_loop;
                    }
                    if (event.Key.Code == sf::Key::F12)
                        frame_advance_mode = !frame_advance_mode;
                    if (frame_advance_mode && event.Key.Code == sf::Key::Space)
                        do_frame = true;
                    break;
                }
                default:;
            }
        }

        if (do_frame)
            doFrame(entity);

        main_window->Clear();

        main_window->Draw(sf::Shape::Line(0, floor_y, 500, floor_y, 1, sf::Color::Red));
        entity->render(main_window);
        main_window->Display();
    }
    break_main_loop:
    return 0;
}


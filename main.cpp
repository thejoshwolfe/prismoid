
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow main_window(sf::VideoMode(800, 600, 32), "grinch");
    main_window.UseVerticalSync(true);

    sf::Image * image = new sf::Image;
    if (!image->LoadFromFile("sample.png"))
        return 1;

    sf::Sprite * sprite = new sf::Sprite(*image);
    //sprite->SetColor(sf::Color(0, 255, 255, 128));
    sprite->SetPosition(200.f, 100.f);
    sprite->SetScale(2.f, 2.f);

    while (main_window.IsOpened()) {
        sf::Event event;
        while (main_window.GetEvent(event)) {
            switch (event.Type) {
                case sf::Event::Closed:
                    main_window.Close();
                    break;
                case sf::Event::Resized:
                    glViewport(0, 0, event.Size.Width, event.Size.Height);
                    break;
                case sf::Event::KeyPressed:
                    // only check for closing here
                    if (event.Key.Code == sf::Key::Escape || (event.Key.Alt && event.Key.Code == sf::Key::F4))
                        main_window.Close();
                    break;
            }
        }

        float elapsed_time = main_window.GetFrameTime();

        // Move the sprite
        const sf::Input * input = &main_window.GetInput();
        if (input->IsKeyDown(sf::Key::Left))  sprite->Move(-100 * elapsed_time, 0);
        if (input->IsKeyDown(sf::Key::Right)) sprite->Move( 100 * elapsed_time, 0);
        if (input->IsKeyDown(sf::Key::Up))    sprite->Move(0, -100 * elapsed_time);
        if (input->IsKeyDown(sf::Key::Down))  sprite->Move(0,  100 * elapsed_time);
        // Rotate the sprite
        if (input->IsKeyDown(sf::Key::Add))      sprite->Rotate(- 100 * elapsed_time);
        if (input->IsKeyDown(sf::Key::Subtract)) sprite->Rotate(+ 100 * elapsed_time);

        main_window.Clear();
        main_window.Draw(*sprite);
        main_window.Display();
    }
    return 0;
}


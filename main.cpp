
#include <SFML/Window.hpp>

void init_opengl();
void draw(float elapsed_time);

int main()
{
    sf::Window main_window(sf::VideoMode(800, 600, 32), "grinch");
    main_window.UseVerticalSync(true);
    sf::Clock clock;
    init_opengl();
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

        draw(clock.GetElapsedTime());

        main_window.Display();
    }
    return 0;
}

void init_opengl()
{
    // Set color and depth clear value
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0f, 1.0f, 1.0f, 500.0f);
}

void draw(float elapsed_time)
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Apply some transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -200.f);
    glRotatef(elapsed_time * 50, 1.f, 0.f, 0.f);
    glRotatef(elapsed_time * 30, 0.f, 1.f, 0.f);
    glRotatef(elapsed_time * 90, 0.f, 0.f, 1.f);

    glBegin(GL_QUADS); {
        glVertex3f(-50.f, -50.f, -50.f);
        glVertex3f(-50.f,  50.f, -50.f);
        glVertex3f( 50.f,  50.f, -50.f);
        glVertex3f( 50.f, -50.f, -50.f);

        glVertex3f(-50.f, -50.f, 50.f);
        glVertex3f(-50.f,  50.f, 50.f);
        glVertex3f( 50.f,  50.f, 50.f);
        glVertex3f( 50.f, -50.f, 50.f);

        glVertex3f(-50.f, -50.f, -50.f);
        glVertex3f(-50.f,  50.f, -50.f);
        glVertex3f(-50.f,  50.f,  50.f);
        glVertex3f(-50.f, -50.f,  50.f);

        glVertex3f(50.f, -50.f, -50.f);
        glVertex3f(50.f,  50.f, -50.f);
        glVertex3f(50.f,  50.f,  50.f);
        glVertex3f(50.f, -50.f,  50.f);

        glVertex3f(-50.f, -50.f,  50.f);
        glVertex3f(-50.f, -50.f, -50.f);
        glVertex3f( 50.f, -50.f, -50.f);
        glVertex3f( 50.f, -50.f,  50.f);

        glVertex3f(-50.f, 50.f,  50.f);
        glVertex3f(-50.f, 50.f, -50.f);
        glVertex3f( 50.f, 50.f, -50.f);
        glVertex3f( 50.f, 50.f,  50.f);
    } glEnd();

}


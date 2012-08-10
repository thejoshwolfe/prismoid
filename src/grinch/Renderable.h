#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Util.h"
#include "SFML/Graphics.hpp"

class Renderable
{
public:
    virtual void render(Vector2 virtual_center, sf::RenderTarget * render_target) = 0;
};

#endif

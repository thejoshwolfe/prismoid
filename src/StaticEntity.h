#ifndef STATICENTITY_H
#define STATICENTITY_H

#include "Entity.h"

class StaticEntity : public Entity
{
private:
    sf::Sprite sprite;
public:
    StaticEntity(const Vector2 & center, const Vector2 & size, const sf::Image &image,
            bool flip_horizontal, bool flip_vertical, bool flip_diagonally) :
        Entity(false, center, size, sf::Color())
    {
        bounding_prismoid.setZ(0, 1);
        std::vector<Vector2> here;
        makeRectangle(&here, center, size);
        for (int i = 0; i < (int)here.size(); i++)
            bounding_prismoid.addEdge(here[i], here[i]);

        sprite.SetImage(image);
        sprite.SetCenter(image.GetWidth() / 2, image.GetHeight() / 2);
        if (flip_diagonally) {
            sprite.SetRotation(-90);
            flip_horizontal = !flip_horizontal;
        }
        sprite.FlipX(flip_horizontal);
        sprite.FlipY(flip_vertical);
    }

    virtual const Vector2& getVelocity() const { return zero; }
    virtual void render(Vector2 virtual_center, sf::RenderTarget * render_target);
private:
    static const Vector2 zero;
};

#endif
